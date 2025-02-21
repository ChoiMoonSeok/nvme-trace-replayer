#include <fcntl.h>
#include <math.h>
#include <inttypes.h>
#include <libnvme.h>

#include "file_reader.h"

#define NUM_OF_GROUP 6


int submit_io_host(nvme_ns_t n, long long int lba, int size, int dt){

    int *data = malloc(size); 

	if (dt > -1){
		int ret = nvme_ns_write(n, data, (lba * 10 + dt) * 512, size);
		// printf("%d %lld %d\n", size / 512, ((lba * 10 + dt) * 512) & 511, (size / 512) & 511);
		if (ret == -1)
			return -1;
	}
	else{
		int ret = nvme_ns_write(n, data, (lba * 10 + NUM_OF_GROUP - 1) * 512, size);
		if (ret == -1)
			return -1;
	}
	nvme_ns_flush(n);

	free(data);

	return 0;
}


int main(){

	nvme_root_t r;
	nvme_host_t h;
	nvme_subsystem_t s, _s;
	nvme_ctrl_t c, _c;
	nvme_path_t p, _p;
	nvme_ns_t n, _n;
	FILE *file;

	r = nvme_scan(NULL);
	if (!r)
		return -1;

	h = nvme_first_host(r);
	s = nvme_first_subsystem(h);
	c = nvme_subsystem_first_ctrl(s);
	n = nvme_ctrl_first_ns(c);

	printf("%s\n", nvme_subsystem_get_name(s));
	printf("%s\n", nvme_subsystem_get_nqn(s));
	printf("%s\n", nvme_ctrl_get_name(c));
	printf("%s\n", nvme_ns_get_name(n));

    file = file_open("modified_hm_0.csv");

	printf("I/O replay start\n");
    
    int io_cnt = 0;
    while (1){

		int ret = 0;
        long long int *line = read_line(file);
        if (line != NULL){
			ret = submit_io_host(n, line[0], (int)line[1], (int)line[2]);
            free(line);
        }
        else
            break;

		if (ret < 0)
			printf("nvme_ns_write fail\n");

        if (io_cnt % 1000 == 0)
	    	nvme_ns_flush(n);
    }

    printf("end\n");

    fclose(file);
	nvme_free_tree(r);

    return 0;
}