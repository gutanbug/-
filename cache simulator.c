#include <stdio.h>
#include <stdlib.h>

struct cache{
	int tag;
	int valid;
	int time;
};
struct cache *cp;

int total, miss;
int lru_time;

int vict(int set, int assoc){
	int i, t = 0;
	int min = lru_time+1, index = 0;
	
	for(i=0;i<assoc;i++){
		t = cp[set*assoc+i].time;
		if(min > t){
			min = t;
			index = i;
		}
	}
	return index;
}

void read_data(int addr, int cache, int block, int assoc){
	int set_count, set, i;
	int size = 10, victim;
	struct cache *temp;
	
	set_count = cache / (block*assoc);
	set = (addr/block) % set_count;
	
	for(i=0;i<assoc;i++){
		temp = &cp[set*assoc + i];
		if(temp->valid == 1 && temp->tag == (addr/block)/set_count){
			temp->time = lru_time;
			return;
		}else if(temp->valid == 0){
			if(i < size)
				size = i;
		}
	}
	miss++;
	if(size == 10){
		victim = vict(set, assoc);
		temp = &cp[set*assoc + victim];
		
		temp->valid = 1;
		temp->time = lru_time;
		temp->tag = (addr/block) / set_count;
	}else{
		temp = &cp[set*assoc+size];
		
		temp->valid = 1;
		temp->time = lru_time;
		temp->tag = (addr/block)/set_count;
	}
}

void simul(int cache, int block, int assoc){
	total=0;
	miss=0;
    int empty, addr;
	int num = cache / block;
	double miss_rate = 0;
    char mode;
    FILE *fp = NULL;
	
	cp = (struct cache*)calloc(num, sizeof(struct cache));

    fp = fopen("gcc.trace", "r");
    while(!feof(fp)){
        fscanf(fp, "%c %x %d\n", &mode, &addr, &empty);
    	
		switch(mode){
			case 'l':
				read_data(addr, cache, block, assoc);
				total++;
				break;
		}
		lru_time++;
    }
	miss_rate = (double)miss/(double)total;
	
	printf("\ncache size : %d\n", cache);
	printf("block size : %d\n", block);
	printf("associative : %d\n", assoc);
	printf("======================\n");
	printf("read miss-rate : %.4lf\n\n", miss_rate);
	
	free(cp);
    fclose(fp);
}

int main(){
	int cache, block, assoc;
	int input;
	
	while(input != 3){
		printf("1.Input size\n2.simulate\n3.exit\n:");
		scanf("%d", &input);
		if(input == 1){
			printf("cache size(1024,2048,4096,8192)\n: ");
			scanf("%d", &cache);
			printf("block size(16,64)\n: ");
			scanf("%d", &block);
			printf("associative(1,2,4,8)\n: ");
			scanf("%d", &assoc);
			printf("\nInput Success !\n\n");
		}
		else if(input == 2)
			simul(cache, block, assoc);
		else
			exit(0);
	}
	return 0;
}
