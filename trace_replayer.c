#include <fcntl.h>
#include <math.h>
#include <inttypes.h>
#include <libnvme.h>

#include "file_reader.h"

#define NUM_OF_GROUP 6


int submit_io_host(int fd, long long int lba, int size, int dt){

    int *data = malloc(sizeof(int) * size / 4); 

    struct nvme_io_args args = {
		.args_size = sizeof(args),
		.fd = fd,
		.nsid = 1,
        .nlb = 1,
		.control = 0,
		.dsm = 0,
		.dspec = 0,
		.reftag = 0,
		.apptag = 0,
		.appmask = 0,
		.storage_tag = 0,
		.data_len = sizeof(int) * size / 4,
		.data = data,
	 	.metadata_len = 0,
		.metadata = NULL,
		.timeout = NVME_DEFAULT_IOCTL_TIMEOUT,
		.result = NULL,
	};
	// printf("%lld %d dt %d\n", lba, size, dt);
	if (dt > -1)
		args.slba = (uint64_t)(lba * 10 + dt);
	else
		args.slba = (uint64_t)(lba * 10 + NUM_OF_GROUP - 1);

	// args.slba = (uint64_t)10000;
	
	// printf("%lld %d\n", lba * 10 + NUM_OF_GROUP - 1, args.data_len);
	nvme_write(&args);
	nvme_flush(fd, 1);

	free(data);

	return 0;
}

int main(){

	int fd;
	FILE *file;


	fd = open("/dev/nvme0n1", O_RDWR);
    if (fd == -1){
		printf("nvme open fail\n");
		exit(1);
	}

    file = file_open("modified_hm_0.csv");

	printf("I/O replay start\n");
    for (int i = 0; i < 1; i++){

        long long int *line = read_line(file);
        if (line != NULL){
			submit_io_host(fd, line[0], (int)line[1], (int)line[2]);
            free(line);
        }
        else
            break;
    }

    printf("end\n");

    fclose(file);

    return 0;
}
