
#define _GNU_SOURCE

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../module_interface.h"

typedef enum {
	InNoRoutes,
  InTruncatedPkts,
  InMcastPkts,
  OutMcastPkts,
  InBcastPkts,
  OutBcastPkts,
  InOctets,
  OutOctets,
  InMcastOctets,
  OutMcastOctets,
	InBcastOctets,
	OutBcastOctets,
	ReasmOverlaps,
	max_stats,
} tcp;

const char *module_get_name(void)
{
	return "nets";
}


const module_stat_def_t *module_get_stat_definitions(void)
{
	static const module_stat_def_t stat_defs[] = {
		{ "InNoRoutes", uinteger,
			"see the procfs(5) manual page for /proc/[pid]/net/netstat" },
		{ "InTruncatedPkts", uinteger,
		  "see the procfs(5) manual page for /proc/[pid]/net/netstat" },
		{ "InMcastPkts", uinteger,
		  "to be written" },
		{ "OutMcastPkts", uinteger,
		  "The internal status of the socket"},
		{ "InBcastPkts", uinteger,
		  "outgoing data queue in terms of kernel memory usage" },
		{ "OutBcastPkts", uinteger,
      "incoming data queue in terms of kernel memory usage" },
		{ "InOctets", uinteger,
		  "Holds internal information about the kernel socket state and is useful only for debugging" },
    { "OutOctets",uinteger,
      "Holds internal information about the kernel socket state and is useful only for debugging"},
    { "InMcastOctets",uinteger,
      "Holds internal information about the kernel socket state and is useful only for debugging"},
    { "OutMcastOctets",uinteger,
      "holds the effective UID of the creator of the socket"},
		{ "InBcastOctets",uinteger,
	     "holds the effective UID of the creator of the socket"},
		{ "OutBcastOctets",uinteger,
	 	   "holds the effective UID of the creator of the socket"},
	  { "ReasmOverlaps",uinteger,
	 	   "holds the effective UID of the creator of the socket"}

	};

	return stat_defs;
}

bool module_init(module_state_t *state)
{
	return true;
}

void module_destroy(module_state_t *state)
{
}

unsigned int module_get_all_stats(module_state_t *state,
                                  module_stat_value_t **stats, const pid_t pid)
{
	char *sysfile = NULL;
	//asprintf(&sysfile, "/proc/%d/nets", pid);
		asprintf(&sysfile, "/proc/self/net/netstat");
	FILE *netsfile = fopen(sysfile, "r");

	if (!netsfile) {    //io ma je rite bbatave che ej rite tcp wala ma pan gothvelu che? no..to aa file kem read karavani e sikh net par thi..
		fprintf(stderr, // io walu jls nu run karine jo output study kar enu ne output kem lavanu e sikh..
		        "Unable to get all stats in module networks, failed to open"
		        " %s: %s\n",
		        sysfile, strerror(errno));
		free(sysfile);

		return 0;
	}

	//free(sysfile);

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	*stats = malloc(sizeof(module_stat_value_t) * max_stats);

	for (stat_t i = InNoRoutes; //ahiya thi reading start thase karwanu che maybe
	     - 1 != (nread = getline(&line, &len, netsfile)) && i != max_stats; i++) { //aa pela run kari jo..pchi sahil nu log lagavje..
		char *l = line;
		char *saveptr = NULL;

		strtok_r(l, ":", &saveptr);
		char *val = strtok_r(NULL, ":", &saveptr);
		if (val == NULL) {
			fprintf(stderr, "Unable to get all stats in module networks, failed to "
			                "parse procfs output.\n");
			free(*stats);
			free(line);
			free(netsfile);

			return 0;
		}

		uintmax_t v = strtoumax(val, NULL, 10); // convert the character string to integer value type.

		(*stats)[i] =
		    (module_stat_value_t){ .def = &module_get_stat_definitions()[i],
			                       .value = (void *)v };// v ma te val store karaivu..bas khali sahil logic ape etle hu ama nakhi dais its easy..

	}

	free(line);
	fclose(netsfile);

	return max_stats;
}
