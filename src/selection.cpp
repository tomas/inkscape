/*
 * Per-desktop selection container
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   MenTaLguY <mental@rydia.net>
 *   bulia byak <buliabyak@users.sf.net>
 *   Andrius R. <knutux@gmail.com>
 *   Abhishek Sharma
 *
 * Copyright (C)      2006 Andrius R.
 * Copyright (C) 2004-2005 MenTaLguY
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2001-2002 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "macros.h"
#include "inkscape.h"
#include "document.h"
#include "desktop.h"
#include "layer-model.h"
#include "selection.h"
#include <2geom/rect.h>
#include "xml/repr.h"
#include "preferences.h"

#include "sp-shape.h"
#include "sp-path.h"
#include "sp-item-group.h"
#include "box3d.h"
#include "box3d.h"
#include "persp3d.h"
#include "ui/tools/node-tool.h"
#include "ui/tool/multi-path-manipulator.h"
#include "ui/tool/path-manipulator.h"
#include "ui/tool/control-point-selection.h"
#include "sp-path.h"
#include "sp-defs.h"

#include <sigc++/functors/mem_fun.h>

#define SP_SELECTION_UPDATE_PRIORITY (G_PRIORITY_HIGH_IDLE + 1)

namespace Inkscape {

Selection::Selection(LayerModel *layers, SPDesktop *desktop) :
    _objs(std::list<SPObject*>()),
    _objs_vector(std::vector<SPObject*>()),
    _objs_set(std::set<SPObject*>()),
    _reprs(std::vector<XML::Node*>()),
    _items(std::vector<SPItem*>()),
    _layers(layers),
    _desktop(desktop),
    _selection_context(NULL),
    _flags(0),
    _idle(0)
{
}

Selection::~Selection() {
    _clear();
    _layers = NULL;
    if (_idle) {
        g_source_remove(_idle);
        _idle = 0;
    }
}

/* Handler for selected objects "modified" signal */

void Selection::_schedule_modified(SPObject */*obj*/, guint flags) {
    if (!this->_idle) {
        /* Request handling to be run in _idle loop */
        this->_idle = g_idle_add_full(SP_SELECTION_UPDATE_PRIORITY, GSourceFunc(&Selection::_emit_modified), this, NULL);
    }

    /* Collect all flags */
    this->_flags |= flags;
}

gboolean
Selection::_emit_modified(Selection *selection)
{
    /* force new handler to be created if requested before we return */
    selection->_idle = 0;
    guint flags = selection->_flags;
    selection->_flags = 0;

    selection->_emitModified(flags);

    /* drop this handler */
    return FALSE;
}

void Selection::_emitModified(guint flags) {
    INKSCAPE.selection_modified(this, flags);
    _modified_signal.emit(this, flags);
}

void Selection::_emitChanged(bool persist_selection_context/* = false */) {
    if (persist_selection_context) {
        if (NULL == _selection_context) {
            _selection_context = _layers->currentLayer();
            sp_object_ref(_selection_context, NULL);
            _context_release_connection = _selection_context->connectRelease(sigc::mem_fun(*this, &Selection::_releaseContext));
        }
    } else {
        _releaseContext(_selection_context);
    }

    INKSCAPE.selection_changed(this);
    _changed_signal.emit(this);
}

void
Selection::_releaseContext(SPObject *obj)
{
    if (NULL == _selection_context || _selection_context != obj)
        return;

    _context_release_connection.disconnect();

    sp_object_unref(_selection_context, NULL);
    _selection_context = NULL;
}

void Selection::_invalidateCachedLists() {
    _items.clear();
    _reprs.clear();
    _objs_vector.clear();
}

void Selection::_clear() {
    _invalidateCachedLists();
    while (!_objs.empty()) {
        SPObject *obj=_objs.front();
        _remove(obj);
    }
}

SPObject *Selection::activeContext() {
    if (NULL != _selection_context)
        return _selection_context;
    return _layers->currentLayer();
    }

bool Selection::includes(SPObject *obj) const {
    if (obj == NULL)
        return FALSE;

    g_return_val_if_fail(SP_IS_OBJECT(obj), FALSE);

    return ( _objs_set.find(obj)!=_objs_set.end() );
}

void Selection::add(SPObject *obj, bool persist_selection_context/* = false */) {
    g_return_if_fail(obj != NULL);
    g_return_if_fail(SP_IS_OBJECT(obj));
    g_return_if_fail(obj->document != NULL);

    if (includes(obj)) {
        return;
    }

    _invalidateCachedLists();
    _add(obj);
    _emitChanged(persist_selection_context);
}

void Selection::add_3D_boxes_recursively(SPObject *obj) {
    std::list<SPBox3D *> boxes = box3d_extract_boxes(obj);

    for (std::list<SPBox3D *>::iterator i = boxes.begin(); i != boxes.end(); ++i) {
        SPBox3D *box = *i;
        _3dboxes.push_back(box);
    }
}

void Selection::_add(SPObject *obj) {
    // unselect any of the item's ancestors and descendants which may be selected
    // (to prevent double-selection)
    _removeObjectDescendants(obj);
    _removeObjectAncestors(obj);
    g_return_if_fail(SP_IS_OBJECT(obj));

    _objs.push_front(obj);
    _objs_set.insert(obj);

    add_3D_boxes_recursively(obj);

    _release_connections[obj] = obj->connectRelease(sigc::mem_fun(*this, (void (Selection::*)(SPObject *))&Selection::remove));
    _modified_connections[obj] = obj->connectModified(sigc::mem_fun(*this, &Selection::_schedule_modified));
}

void Selection::set(SPObject *object, bool persist_selection_context) {
    _clear();
    add(object, persist_selection_context);
}

void Selection::toggle(SPObject *obj) {
    if (includes (obj)) {
        remove (obj);
    } else {
        add(obj);
    }
}

void Selection::remove(SPObject *obj) {
    g_return_if_fail(obj != NULL);
    g_return_if_fail(SP_IS_OBJECT(obj));
    g_return_if_fail(includes(obj));

    _invalidateCachedLists();
    _remove(obj);
    _emitChanged();
}

void Selection::remove_3D_boxes_recursively(SPObject *obj) {
    std::list<SPBox3D *> boxes = box3d_extract_boxes(obj);

    for (std::list<SPBox3D *>::iterator i = boxes.begin(); i != boxes.end(); ++i) {
        SPBox3D *box = *i;
        std::list<SPBox3D *>::iterator b = std::find(_3dboxes.begin(), _3dboxes.end(), box);
        if (b == _3dboxes.end()) {
            g_print ("Warning! Trying to remove unselected box from selection.\n");
            return;
        }
        _3dboxes.erase(b);
    }
}

void Selection::_remove(SPObject *obj) {
    _modified_connections[obj].disconnect();
    _modified_connections.erase(obj);

    _release_connections[obj].disconnect();
    _release_connections.erase(obj);

    remove_3D_boxes_recursively(obj);

    _objs.remove(obj);
    _objs_set.erase(obj);
}

void Selection::setList(std::vector<SPItem*> const &list) {
    // Clear and add, or just clear with emit.
    if (!list.empty()) {
        _clear();
        addList(list);
    } else clear();
}

void Selection::addList(std::vector<SPItem*> const &list) {

    if (list.empty())
        return;

    _invalidateCachedLists();

    for ( std::vector<SPItem*>::const_iterator iter=list.begin();iter!=list.end(); ++iter) {
        SPObject *obj = *iter;
        if (includes(obj)) continue;
        _add (obj);
    }

    _emitChanged();
}

void Selection::setReprList(std::vector<XML::Node*> const &list) {
    _clear();

    for ( std::vector<XML::Node*>::const_reverse_iterator iter=list.rbegin();iter!=list.rend(); ++iter) {
        SPObject *obj=_objectForXMLNode(*iter);
        if (obj) {
            _add(obj);
        }
    }

    _emitChanged();
}

void Selection::clear() {
    _clear();
    _emitChanged();
}

std::vector<SPObject*> const &Selection::list() {
    if(!_objs_vector.empty())
    return _objs_vector;

    for ( std::list<SPObject*>::const_iterator iter=_objs.begin();iter!=_objs.end(); ++iter) {
            _objs_vector.push_back(*iter);
    }
    return _objs_vector;

}

std::vector<SPItem*> const &Selection::itemList() {
    if (!_items.empty()) {
        return _items;
    }

    for ( std::list<SPObject*>::const_iterator iter=_objs.begin();iter!=_objs.end(); ++iter) {
        SPObject *obj=*iter;
        if (SP_IS_ITEM(obj)) {
            _items.push_back(SP_ITEM(obj));
        }
    }
    return _items;
}

std::vector<XML::Node*> const &Selection::reprList() {
    if (!_reprs.empty()) { return _reprs; }
    std::vector<SPItem*> list = itemList();
    for ( std::vector<SPItem*>::const_iterator iter=list.begin();iter!=list.end(); ++iter) {
        SPObject *obj = *iter;
        _reprs.push_back(obj->getRepr());
    }
    return _reprs;
}

std::list<Persp3D *> const Selection::perspList() {
    std::list<Persp3D *> pl;
    for (std::list<SPBox3D *>::iterator i = _3dboxes.begin(); i != _3dboxes.end(); ++i) {
        Persp3D *persp = box3d_get_perspective(*i);
        if (std::find(pl.begin(), pl.end(), persp) == pl.end())
            pl.push_back(persp);
    }
    return pl;
}

std::list<SPBox3D *> const Selection::box3DList(Persp3D *persp) {
    std::list<SPBox3D *> boxes;
    if (persp) {
        for (std::list<SPBox3D *>::iterator i = _3dboxes.begin(); i != _3dboxes.end(); ++i) {
            SPBox3D *box = *i;
            if (persp == box3d_get_perspective(box)) {
                boxes.push_back(box);
            }
        }
    } else {
        boxes = _3dboxes;
    }
    return boxes;
}

SPObject *Selection::single() {
    if ( _objs.size() == 1 ) {
        return _objs.front();
    } else {
        return NULL;
    }
}

SPItem *Selection::singleItem() {
    std::vector<SPItem*> const items=itemList();
    if ( items.size()==1) {
        return items[0];
    } else {
        return NULL;
    }
}

SPItem *Selection::smallestItem(Selection::CompareSize compare) {
    return _sizeistItem(true, compare);
}

SPItem *Selection::largestItem(Selection::CompareSize compare) {
    return _sizeistItem(false, compare);
}

SPItem *Selection::_sizeistItem(bool sml, Selection::CompareSize compare) {
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
    gdouble max = sml ? 1e18 : 0;
    SPItem *ist = NULL;

    for ( std::vector<SPItem*>::const_iterator i=items.begin();i!=items.end(); ++i) {
        Geom::OptRect obox = SP_ITEM(*i)->desktopPreferredBounds();
        if (!obox || obox.empty()) continue;
        Geom::Rect bbox = *obox;

        gdouble size = compare == 2 ? bbox.area() :
            (compare == 1 ? bbox.width() : bbox.height());
        size = sml ? size : size * -1;
        if (size < max) {
            max = size;
            ist = SP_ITEM(*i);
        }
    }
    return ist;
}

Inkscape::XML::Node *Selection::singleRepr() {
    SPObject *obj=single();
    return obj ? obj->getRepr() : NULL;
}

Geom::OptRect Selection::bounds(SPItem::BBoxType type) const
{
    return (type == SPItem::GEOMETRIC_BBOX) ?
        geometricBounds() : visualBounds();
}

Geom::OptRect Selection::geometricBounds() const
{
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();

    Geom::OptRect bbox;
    for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
        bbox.unionWith(SP_ITEM(*iter)->desktopGeometricBounds());
    }
    return bbox;
}

Geom::OptRect Selection::visualBounds() const
{
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();

    Geom::OptRect bbox;
    for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
        bbox.unionWith(SP_ITEM(*iter)->desktopVisualBounds());
    }
    return bbox;
}

Geom::OptRect Selection::preferredBounds() const
{
    if (Inkscape::Preferences::get()->getInt("/tools/bounding_box") == 0) {
        return bounds(SPItem::VISUAL_BBOX);
    } else {
        return bounds(SPItem::GEOMETRIC_BBOX);
    }
}

Geom::OptRect Selection::documentBounds(SPItem::BBoxType type) const
{
    Geom::OptRect bbox;
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
    if (items.empty()) return bbox;

    for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
        SPItem *item = SP_ITEM(*iter);
        bbox |= item->documentBounds(type);
    }

    return bbox;
}

// If we have a selection of multiple items, then the center of the first item
// will be returned; this is also the case in SelTrans::centerRequest()
boost::optional<Geom::Point> Selection::center() const {
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
    if (!items.empty()) {
        SPItem *first = items.back(); // from the first item in selection
        if (first->isCenterSet()) { // only if set explicitly
            return first->getCenter();
        }
    }
    Geom::OptRect bbox = preferredBounds();
    if (bbox) {
        return bbox->midpoint();
    } else {
        return boost::optional<Geom::Point>();
    }
}

std::vector<Inkscape::SnapCandidatePoint> Selection::getSnapPoints(SnapPreferences const *snapprefs) const {
    std::vector<Inkscape::SnapCandidatePoint> p;

    if (snapprefs != NULL){
        SnapPreferences snapprefs_dummy = *snapprefs; // create a local copy of the snapping prefs
        snapprefs_dummy.setTargetSnappable(Inkscape::SNAPTARGET_ROTATION_CENTER, false); // locally disable snapping to the item center
        std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
        for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
            SPItem *this_item = *iter;
            this_item->getSnappoints(p, &snapprefs_dummy);
            
            //Include the transformation origin for snapping
            //For a selection or group only the overall center is considered, not for each item individually
            if (snapprefs->isTargetSnappable(Inkscape::SNAPTARGET_ROTATION_CENTER)) {
                p.push_back(Inkscape::SnapCandidatePoint(this_item->getCenter(), SNAPSOURCE_ROTATION_CENTER));
            }
        }
    }

    return p;
}

void Selection::_removeObjectDescendants(SPObject *obj) {
    std::vector<SPObject*> toremove;
    for ( std::list<SPObject*>::const_iterator iter=_objs.begin();iter!=_objs.end(); ++iter) {
        SPObject *sel_obj= dynamic_cast<SPObject*>(*iter);
        SPObject *parent = sel_obj->parent;
        while (parent) {
            if ( parent == obj ) {
                toremove.push_back(sel_obj);
                break;
            }
            parent = parent->parent;
        }
    }
    for ( std::vector<SPObject*>::const_iterator iter=toremove.begin();iter!=toremove.end(); ++iter) {
        _remove(*iter);
    }
}

void Selection::_removeObjectAncestors(SPObject *obj) {
        SPObject *parent = obj->parent;
        while (parent) {
            if (includes(parent)) {
                _remove(parent);
            }
            parent = parent->parent;
        }
}

SPObject *Selection::_objectForXMLNode(Inkscape::XML::Node *repr) const {
    g_return_val_if_fail(repr != NULL, NULL);
    gchar const *id = repr->attribute("id");
    g_return_val_if_fail(id != NULL, NULL);
    SPObject *object=_layers->getDocument()->getObjectById(id);
    g_return_val_if_fail(object != NULL, NULL);
    return object;
}

size_t Selection::numberOfLayers() {
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
    std::set<SPObject*> layers;
    for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
        SPObject *layer = _layers->layerForObject(*iter);
        layers.insert(layer);
    }
    return layers.size();
}

size_t Selection::numberOfParents() {
    std::vector<SPItem*> const items = const_cast<Selection *>(this)->itemList();
    std::set<SPObject*> parents;
    for ( std::vector<SPItem*>::const_iterator iter=items.begin();iter!=items.end(); ++iter) {
        SPObject *parent = (*iter)->parent;
        parents.insert(parent);
    }
    return parents.size();
}

void
Selection::emptyBackup(){
    _selected_ids.clear();
    _seldata.clear();
    params.clear();
}

void
Selection::setBackup () 
{
    SPDesktop *desktop = SP_ACTIVE_DESKTOP;
    if (desktop) {
        std::vector<SPItem*> selected_items = itemList();
        _selected_ids.clear();
        _seldata.clear();
        params.clear();
        for(std::vector<SPItem*>::const_iterator x = selected_items.begin(); x != selected_items.end(); ++x){
            std::string selected_id;
            selected_id += "--id=";
            selected_id += (*x)->getId();
            params.push_front(selected_id);
            _selected_ids.push_back((*x)->getId());
        }
        Inkscape::UI::Tools::NodeTool *tool = 0;
        
        Inkscape::UI::Tools::ToolBase *ec = desktop->event_context;
        if (INK_IS_NODE_TOOL(ec)) {
            tool = static_cast<Inkscape::UI::Tools::NodeTool*>(ec);
        }
        if(tool){
            Inkscape::UI::ControlPointSelection *cps = tool->_selected_nodes;
            std::list<Inkscape::UI::SelectableControlPoint *> points_list = cps->_points_list;
            for (std::list<Inkscape::UI::SelectableControlPoint *>::iterator i = points_list.begin(); i != points_list.end(); ++i) {
                Inkscape::UI::Node *node = dynamic_cast<Inkscape::UI::Node*>(*i);
                if (node) { 
                    std::string id = node->nodeList().subpathList().pm().item()->getId(); 

                    int sp = 0;
                    bool found_sp = false;
                    for(Inkscape::UI::SubpathList::iterator i = node->nodeList().subpathList().begin(); i != node->nodeList().subpathList().end(); ++i,++sp){
                        if(&**i == &(node->nodeList())){
                            found_sp = true;
                            break;
                        }
                    }
                    int nl=0;
                    bool found_nl = false;
                    for (Inkscape::UI::NodeList::iterator j = node->nodeList().begin(); j != node->nodeList().end(); ++j, ++nl){
                        if(&*j==node){
                            found_nl = true;
                            break;
                        }
                    }
                    std::ostringstream ss;
                    ss<< "--selected-nodes=" << id << ":" << sp << ":" << nl;
                    Glib::ustring selected_nodes = ss.str();

                    if(found_nl && found_sp) {
                        _seldata.push_back(std::make_pair(id,std::make_pair(sp,nl)));
                        params.push_back(selected_nodes);
                    } else {
                        g_warning("Something went wrong while trying to pass selected nodes to extension. Please report a bug.");
                    }
                }
            }
        }
    }//end add selected nodes
}

void
Selection::restoreBackup() 
{
    SPDesktop *desktop = SP_ACTIVE_DESKTOP;
    if (desktop) {
        Inkscape::UI::Tools::NodeTool *tool = 0;
        Inkscape::UI::Tools::ToolBase *ec = desktop->event_context;
        if (INK_IS_NODE_TOOL(ec)) {
            tool = static_cast<Inkscape::UI::Tools::NodeTool*>(ec);
        }
        clear();
        std::vector<std::string>::reverse_iterator rit = _selected_ids.rbegin();
        for (; rit!= _selected_ids.rend(); ++rit){
            SPObject * obj = desktop->doc()->getObjectById(rit->c_str());
            SPDefs * defs = desktop->getDocument()->getDefs();
            if (obj && !defs->isAncestorOf(obj)) {
                add(obj);
            }
        }
        if (tool) {
            Inkscape::UI::ControlPointSelection *cps = tool->_selected_nodes;
            cps->selectAll();
            std::list<Inkscape::UI::SelectableControlPoint *> points_list = cps->_points_list;
            cps->clear();
            Inkscape::UI::Node * node = dynamic_cast<Inkscape::UI::Node*>(*points_list.begin());
            if (node) {
                Inkscape::UI::SubpathList sp = node->nodeList().subpathList();
                for (std::vector<std::pair<std::string, std::pair<int, int> > >::iterator l = _seldata.begin(); l != _seldata.end();  ++l) {
                    SPPath * path = dynamic_cast<SPPath *>(desktop->doc()->getObjectById(l->first));
                    gint sp_count = 0;
                    for (Inkscape::UI::SubpathList::iterator j = sp.begin(); j != sp.end(); ++j, ++sp_count) {
                        if(sp_count == l->second.first) {
                            gint nt_count = 0;
                            for (Inkscape::UI::NodeList::iterator k = (*j)->begin(); k != (*j)->end(); ++k, ++nt_count) {
                                if(nt_count == l->second.second) {
                                    k->select(true);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
            points_list.clear();
        }
    }
}


}

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
