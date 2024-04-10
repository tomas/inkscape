/*
 * SVG <inkscape:tagref> implementation
 *
 * Authors:
 *   Theodore Janeczko
 *   Liam P White
 *
 * Copyright (C) Theodore Janeczko 2012-2014 <flutterguy317@gmail.com>
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <cstring>
#include <string>

#include <glibmm/i18n.h>
#include "display/drawing-group.h"
#include "attributes.h"
#include "document.h"
#include "uri.h"
#include "xml/repr.h"
#include "preferences.h"
#include "style.h"
#include "sp-factory.h"
#include "sp-symbol.h"
#include "sp-tag-use.h"
#include "sp-tag-use-reference.h"

SPTagUse::SPTagUse()
{
    href = NULL;
    //new (_changed_connection) sigc::connection;
    ref = new SPTagUseReference(this);
    
    _changed_connection = ref->changedSignal().connect(sigc::mem_fun(*this, &SPTagUse::href_changed));
}

SPTagUse::~SPTagUse()
{
    ref->detach();
    delete ref;
    ref = 0;
}

void
SPTagUse::build(SPDocument *document, Inkscape::XML::Node *repr)
{
    SPObject::build(document, repr);
    readAttr( "xlink:href" );

    // We don't need to create child here:
    // reading xlink:href will attach ref, and that will cause the changed signal to be emitted,
    // which will call sp_tag_use_href_changed, and that will take care of the child
}

void
SPTagUse::release()
{
    _changed_connection.disconnect();

    g_free(href);
    href = NULL;

    ref->detach();

    SPObject::release();
}

void
SPTagUse::set(unsigned key, gchar const *value)
{

    switch (key) {
        case SP_ATTR_XLINK_HREF: {
            if ( value && href && ( strcmp(value, href) == 0 ) ) {
                /* No change, do nothing. */
            } else {
                g_free(href);
                href = NULL;
                if (value) {
                    // First, set the href field, because sp_tag_use_href_changed will need it.
                    href = g_strdup(value);

                    // Now do the attaching, which emits the changed signal.
                    try {
                        ref->attach(Inkscape::URI(value));
                    } catch (Inkscape::BadURIException &e) {
                        g_warning("%s", e.what());
                        ref->detach();
                    }
                } else {
                    ref->detach();
                }
            }
            break;
        }

        default:
                SPObject::set(key, value);
            break;
    }
}

Inkscape::XML::Node *
SPTagUse::write(Inkscape::XML::Document *xml_doc, Inkscape::XML::Node *repr, guint flags)
{
    if ((flags & SP_OBJECT_WRITE_BUILD) && !repr) {
        repr = xml_doc->createElement("inkscape:tagref");
    }

    SPObject::write(xml_doc, repr, flags);

    return repr;
}

void
SPTagUse::href_changed(SPObject *old_ref, SPObject *new_ref)
{
    if (old_ref && getRepr()) {
        char const *id = old_ref->getAttribute("id");
        if (id) {
            getRepr()->setAttribute("xlink:href", Glib::ustring("#") + id);
        }
    }
}

SPItem * SPTagUse::get_original()
{
    SPItem *ref_ = NULL;
    if (ref) {
        ref_ = ref->getObject();
    }
    return ref_;
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
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
