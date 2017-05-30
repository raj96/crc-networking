#include <stdio.h>
#include <stdlib.h>

int bin_count;

int* bin(int num,int padding) {

	int tnum = num;
	int count = 1;

	while(tnum>1) {
		tnum = tnum/2;
		count++;
	}

	int total = count + padding;
	bin_count = total;
//	printf("Count: %d\nPadding: %d\n",count,padding);

	int *binary = (int *)malloc(sizeof(int) * total);
	
	count -=1;

//	printf("Number: %d\n",num);
	while(num>1) {
//		printf("%d",num%2);
		*(binary+(count--)) = num%2;
		num /= 2;
	}
	
	*(binary+(count--)) = num%2;

	int pad_index = total - padding;

	while(pad_index<total) 
		*(binary+(pad_index++)) = 0;
	return binary;

}

int* binary_divide(int *data_binary_orig,int data_binary_length,int *divisor_binary,int divisor_binary_length) {
	int exit = 0;
	int offset;
	int *data_binary = (int *)malloc(sizeof(int) * data_binary_length);

	int i = 0;
	while(i<data_binary_length)
		data_binary[i] = data_binary_orig[i++];

	while(!exit) {
		offset = 0;
		while(data_binary[offset]!=1 && offset<data_binary_length)
			offset++;
		if(offset>=data_binary_length || data_binary_length-offset < divisor_binary_length)
			exit = 1;
		else {

			for(int index=0;index<divisor_binary_length;index++) 
				data_binary[index + offset] ^= divisor_binary[index];
		}

	}

	return data_binary;

}

void showbin(int *z,int len,int end_offset) {
	int i= len - end_offset;
//	printf("Length: %d\n",len);
	while(i<len) 
		printf("%d",*(z+(i++)));
	
	printf("\n");
	return;
}

int* get_modded_data(int *data_binary,int data_binary_length,int *crc) {
	int i = 0;
	while(i<data_binary_length) {
		*(crc+i) = *(crc+i)|*(data_binary+i);
		i++;
	}

	return crc;
}

int checkcrc(int *div,int len) {
	int i = 0;
	int res = *(div+(i++));

	while(i<len)
		res |= *(div+(i++));

	return res;
}

int main() {
	int data,divisor;

	printf("Enter data: ");
	scanf("%d",&data);

	printf("Enter divisor: ");
	scanf("%d",&divisor);
	getchar();
	char noise;
	printf("Inject some noise in the channel?(Y/n)\n");
	scanf("%c",&noise);

	int *divisor_binary = bin(divisor,0);
	int divisor_binary_length = bin_count;

	int *data_binary = bin(data,divisor_binary_length-1);
	int *data_binary_copy = data_binary;
	int data_binary_length = bin_count;


	printf("\n\nSender's Side\n--------------------------------------\n");
	int *orig_data = bin(data,0);
	int orig_data_length = bin_count;
	printf("Original Data: \t\t");
	showbin(orig_data,orig_data_length,orig_data_length);

	printf("Divisor: \t\t");
	showbin(divisor_binary,divisor_binary_length,divisor_binary_length);

	int *crc = binary_divide(data_binary,data_binary_length,divisor_binary,divisor_binary_length);
	printf("CRC: \t\t\t");
	showbin(crc,data_binary_length,divisor_binary_length-1);

	int *data_to_be_sent = get_modded_data(data_binary_copy,data_binary_length,crc);
	printf("Modded Data: \t\t");
	showbin(data_to_be_sent,data_binary_length,data_binary_length);

	printf("\n\nReceiver's Side\n--------------------------------------\n");
	if(noise=='y' || noise=='Y') {
		int index[] = {	17332177%data_binary_length,
						8429929%data_binary_length,
						93402%data_binary_length,
						47238999%data_binary_length};
		data_to_be_sent[index[0]] ^= data_to_be_sent[index[0]];
		data_to_be_sent[index[1]] ^= data_to_be_sent[index[1]];
		data_to_be_sent[index[2]] ^= data_to_be_sent[index[2]];
		data_to_be_sent[index[3]] ^= data_to_be_sent[index[3]];

	}
	printf("Received Data: \t\t");
	showbin(data_to_be_sent,data_binary_length,data_binary_length);

	printf("Divisor: \t\t");
	showbin(divisor_binary,divisor_binary_length,divisor_binary_length);

	int *result = binary_divide(data_to_be_sent,data_binary_length,divisor_binary,divisor_binary_length);
	printf("Division Result: \t");
	showbin(result,data_binary_length,data_binary_length);

	int check_flag = checkcrc(result,data_binary_length);

	if(!check_flag)
		printf("Data OK\n");
	else
		printf("Data Not OK\n");

	return 0;

}