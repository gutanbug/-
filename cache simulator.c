#include <stdio.h>
#include <stdlib.h>

typedef struct blk *block_ptr;
typedef struct blk{
	int valid;
	int tag;
	int period;
}blk;
block_ptr bp;

int cycle, miss;
int lru_time;

int find_index(int set, int b_count){
	int i=0, t = 0;
	int min = lru_time+1, index = 0;
	
	while(i != b_count){
		t = bp[set*b_count+i].period;
		if(t < min){
			min = t; index = i;
		}
		i++;
	}
	return index;
}

void read_data(int addr, int cache, int block, int b_count){
	block_ptr temp;
	int set_count = cache / (block*b_count);
	int set = (addr/block) % set_count;
	int size = 10, victim, i;
	
	for(i=0;i<b_count;i++){
		temp = &bp[set*b_count + i];
		if(temp->valid == 1 && temp->tag == (addr/block)/set_count){
			temp->period = lru_time;
			return;
		}else if(temp->valid == 0){
			if(i < size)
				size = i;
		}
	}
	miss++;
	if(size == 10){
		victim = find_index(set, b_count);
		temp = &bp[set*b_count + victim];
		
		temp->valid = 1;
		temp->period = lru_time;
		temp->tag = (addr/block) / set_count;
	}else{
		temp = &bp[set*b_count+size];
		
		temp->valid = 1;
		temp->period = lru_time;
		temp->tag = (addr/block)/set_count;
	}
}

void check(int cache, int block, int b_count){
	FILE *fp = NULL;
	cycle=0; miss=0;
    int empty, addr;
	int num = cache / block;
	double miss_rate = 0;
    char mode;
	
	bp = (block_ptr)calloc(num, sizeof(blk));

    fp = fopen("gcc.trace", "r");
    while(!feof(fp)){
        fscanf(fp, "%c %x %d\n", &mode, &addr, &empty);
		if(mode == 'l'){
			read_data(addr, cache, block, b_count);
			cycle++;
		}
		lru_time++;
    }
	miss_rate = (double)miss/(double)cycle;
	
	printf("\ncache size : %d\n", cache);
	printf("block size : %d\n", block);
	printf("associative : %d\n", b_count);
	printf("======================\n");
	printf("read miss-rate : %.4lf\n\n", miss_rate);
	
	free(bp);
    fclose(fp);
}

int main(){
	int cache, block, b_count;
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
			scanf("%d", &b_count);
			printf("\nInput Success !\n\n");
		}
		else if(input == 2)
			check(cache, block, b_count);
		else
			exit(0);
	}
	return 0;
}
