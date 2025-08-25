#include "gbsp.h"

int		c_nofaces;
int		c_facenodes;

// Only save out planes that are actually used as nodes!

int		planeused[MAX_MAP_PLANES];



/**=============================================
 * 
 * 
 * =============================================
 */

 void SetModelNumbers(void) {
	int		i;
	int		models;
	std::string		val;

	models = 1;
	for(i = 1; i < num_entities; i++) {
		if(entities[i].numbrushes) {
			std::cout << val << " " << models << std::endl;
			models++;
			SetKeyValue(&entities[i], "model", val);
		}
	}
 }

 #define MAX_SWITCHED_LIGHTS 32
 void SetLightStyles(void) {
	int		stylenum;
	std::string		t;
	entity_t		*e;
	int		i, j;
	std::string		val;
	std::vector<std::string>	lighttargets;

	stylenum = 0;
	for(i = 1; i < num_entities; i++) {
		e = &entities[i];

		t = ValueForKey(e, "classname");
		if(G_stringcasecomp(t, "light")) {
			continue;
		}
		t = ValueForKey(e, "targetname");
		if(t == "") {
			continue;
		}
	}
 }