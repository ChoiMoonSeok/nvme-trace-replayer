#include <fcntl.h>
#include <math.h>
#include <inttypes.h>
#include <libnvme.h>

#include "file_reader.h"
#include "HashTable.h"

#define NUM_OF_GROUP 6
#define TRAIN_TRIGGER 100000


int submit_io_host(nvme_ns_t n, long long int lba, int size, int dt){

    int *data = malloc(size); 

	if (dt > -1){
		int ret = nvme_ns_write(n, data, (lba * 10 + dt) * 512, size);
		if (ret == -1)
			return -1;
	}
	else{
		int ret = nvme_ns_write(n, data, (lba * 10 + NUM_OF_GROUP - 1) * 512, size);
		if (ret == -1)
			return -1;
	}

	free(data);

	return 0;
}


int record_io_host(long long int lba, int size, int freq, int dt, char *fname){

	FILE *fp = fopen(fname, "a");
	fprintf(fp, "%lld %d %d %d\n", lba, size, freq, dt);
	fclose(fp);

	return 0;
}


int main(int argc, char *argv[]){

	nvme_root_t r;
	nvme_host_t h;
	nvme_subsystem_t s, _s;
	nvme_ctrl_t c, _c;
	nvme_path_t p, _p;
	nvme_ns_t n, _n;
	FILE *file;

	struct Node *wc_tbl = init_hash_tbl();
	struct Node *freq_tbl = init_hash_tbl();
	struct Node *size_tbl = init_hash_tbl();

	char *fname = argv[1];

	r = nvme_scan(NULL);
	if (!r)
		return -1;

	h = nvme_first_host(r);
	s = nvme_first_subsystem(h);
	c = nvme_subsystem_first_ctrl(s);
	n = nvme_ctrl_first_ns(c);
	int ret = system("python3 train.py");

	printf("-------------------------------------------------------------------\n");
	printf("SSD Infomation\n");
	printf("%s\n", nvme_subsystem_get_name(s));
	printf("%s\n", nvme_subsystem_get_nqn(s));
	printf("%s\n", nvme_ctrl_get_name(c));
	printf("%s\n", nvme_ns_get_name(n));
	printf("-------------------------------------------------------------------\n");

    file = file_open(fname);

	printf("I/O replay start (fname : %s)\n", fname);
    
    int io_cnt = 0;
    while (1){

		++io_cnt;

		int ret = 0;
        long long int *line = read_line(file);
        if (line != NULL){
			ret = submit_io_host(n, line[0], (int)line[1], (int)line[2]);
			struct Node *wc = search_node(wc_tbl, (uint64_t)line[0]);
			if (wc != NULL){
				int dt = io_cnt - wc->val;
				struct Node *size = search_node(size_tbl, (uint64_t)line[0]);
				struct Node *freq = search_node(freq_tbl, (uint64_t)line[0]);

				record_io_host(line[0], size->val, freq->val, dt, "recorded_io.csv");

				wc->val = io_cnt;
				size->val = (int)line[1];
				freq->val++;
			}
			else{
				add_node(wc_tbl, (uint64_t)line[0], io_cnt);
				add_node(freq_tbl, (uint64_t)line[0], 1);
				add_node(size_tbl, (uint64_t)line[0], (int)line[1] / 512);
			}


            free(line);
        }
        else
            break;

		if (ret < 0){
			printf("nvme_ns_write fail\n");
			exit(1);
		}

        if (io_cnt % 1000 == 0)
	    	nvme_ns_flush(n);

		if (io_cnt % TRAIN_TRIGGER == 0){
			int ret = system("python3 train.py");
			if (ret == -1){
				printf("ML train trigger fail (err)\n");
				exit(1);
			}
			else if (ret == 127){
				printf("ML train trigger fail (train exec)\n");
				exit(1);	
			}
		}
    }

    printf("end\n");

    fclose(file);
	nvme_free_tree(r);

    return 0;
}