#include "cib.h"
#include "pmhelper.h"
#include "property.h"
#include "multi_property.h"
#include <net/if.h>
#include <stdio.h>
#include "jansson.h"

void generate_cib_from_ifaces ()
{
    struct if_nameindex *if_indexes, *iface;
    json_t *json_file = json_array();

    property_t *iface_prop = property_init();
    iface_prop->key = "interface";
    iface_prop->value = calloc(1, sizeof(value_t));
    iface_prop->precedence = 2;

    property_t *local_iface_prop = property_init();
    local_iface_prop->key = "local_interface";
    local_iface_prop->precedence = 2;
    local_iface_prop->value = calloc(1, sizeof(value_t));
    local_iface_prop->value->boolean = true;

    add_property(iface_prop, local_iface_prop);

    if_indexes = if_nameindex();
    if ( if_indexes != NULL )
    {
        for (iface = if_indexes; iface->if_index != 0 || iface->if_name != NULL; iface++)
        {
            printf("%s\n", iface->if_name);
            json_object_set(json_file, "uid", json_string(iface->if_name));
            json_object_set(json_file, "description",
                            json_string(concat("autogenerated CIB node for local interface ", iface->if_name)));
            json_object_set(json_file, "root", json_boolean(true));
            json_object_set(json_file, "filename", json_string(concat(iface->if_name, ".cib")));
            json_object_set(json_file, "expire", json_integer(-1));
            iface_prop->value->string = iface->if_name;
            //TODO: property_to_json(property_t *)
            //so that following is possible: json_object_set(json_file, "properties", property_to_json(iface_prop));

            write_json_file(concat(concat("/home/samulars/", iface->if_name), ".cib"), json_file); //TODO: Change to working folder

            /*
            ------ FROM resthelper.py FOR REFERENCE ------
            c = cib.CIBNode()
            c.uid = en
            c.description = "autogenerated CIB node for local interface %s" % en
            c.root = True
            c.filename = en + '.cib'
            c.expire = -1

            pa = PropertyArray()
            pa.add(NEATProperty(('interface', en), precedence=NEATProperty.IMMUTABLE))
            pa.add(NEATProperty(('local_interface', True), precedence=NEATProperty.IMMUTABLE))
            c.properties.add(pa)

            addr_list = []

            for af, addresses in netifaces.ifaddresses(en).items():
                if af == netifaces.AF_INET:
                    af_prop = NEATProperty(('ip_version', 4), precedence=NEATProperty.IMMUTABLE)
                elif af == netifaces.AF_INET6:
                    af_prop = NEATProperty(('ip_version', 6), precedence=NEATProperty.IMMUTABLE)
                else:
                    continue

                for addr in addresses:
                    pa = PropertyArray()
                    pa.add(NEATProperty(('local_ip', addr.get('addr', 0)), precedence=NEATProperty.IMMUTABLE))
                    pa.add(af_prop)
                    addr_list.append(pa)

            if addr_list:
                c.properties.add(addr_list)
                yield c.json()
            */
        }
    }

    return ;
}

int main(int argc, char const *argv[])
{
    generate_cib_from_ifaces();
    return 0;
}
