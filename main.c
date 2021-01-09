#include <stdio.h>
#include <stdlib.h>


#define STB_IMAGE_IMPLEMENTATION
#include "./.h/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./.h/stb_image_write.h"
#include<math.h>
#include<string.h>
long int sum_arr(int array[], int length_arr)
{
	//len_array = sizeof(array)/sizeof(array[0]);
	long int sum = 0;
	for(int i=0; i<length_arr;i++)
	{
		sum = sum + array[i];
	}
	return(sum);
}

double find_corr(int arr_a[], int arr_b[], int length_arr)
{
	const int n = length_arr;
	int xi_yi[n];
	for(int i = 0; i<n; i++)
	{
		xi_yi[i] = arr_a[i]*arr_b[i];
	}
	long int sum_x_y = sum_arr(xi_yi,n); //cal sum xi*yi
	long int sum_x = sum_arr(arr_a,n);
	long int sum_y = sum_arr(arr_b,n);
	int x_square[n];
	int y_square[n];
	for(int i = 0; i<n;i++)
	{
		x_square[i] = arr_a[i]*arr_a[i];
		y_square[i] = arr_b[i]*arr_b[i];
	}
	long int sum_x_square = sum_arr(x_square,n);
	long int sum_y_square = sum_arr(y_square,n);
	long int square_sum_x = sum_x*sum_x;
	long int square_sum_y = sum_y*sum_y;
	double corr_x_y;
	long int nominator = n*sum_x_y - sum_x*sum_y;
	double denominator_1 = sqrt(n*sum_x_square - square_sum_x);
	double denominator_2 = sqrt(n*sum_y_square - square_sum_y);
	corr_x_y = (double)nominator/(denominator_1*denominator_2);
	return(corr_x_y);
}

int main()
{
	char file[20];
	char template[20];
	char result[20];
	puts("file name, temp name, result name");
	scanf("%19s", file);
	scanf("%19s", template);
	scanf("%19s", result);
	
	char file_path[40] = "./image/";
	char temp_path[40]="./template/";
	char result_path[40]="./result/";
	strcat(file_path, file);
	strcat(temp_path,template);
	strcat(result_path,result);

	int w_i, h_i,w_t, h_t, c_i, c_t;
	unsigned char *img = stbi_load(file_path, &w_i, &h_i, &c_i, 0);
	unsigned char *temp = stbi_load(temp_path, &w_t, &h_t, &c_t, 0);
	int size_img = (h_i*w_i*c_i);
	int temp_size = (h_t*w_t*c_t); //??????????
	unsigned char *out_img  = malloc(size_img*3);
	int d_h = h_i - h_t;
	int d_w = w_i - w_t;

	int red[size_img];
	int red_temp[temp_size];

	puts("Loading...");
	if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    if(temp == NULL) {
        printf("Error in loading the template\n");
        exit(1);
    }
	printf("Image size: %dx%d\nTemplate size: %dx%d\n",w_i,h_i,w_t,h_t);
	puts("Processing...\n");

	int temp_arr[temp_size];
	for(int i=0;i<temp_size;i++)
	{
		temp_arr[i] = temp[i];
	}


	double red_corr[d_h][d_w];
	
	for(int i=0;i<d_h;i++)
	{
		for(int j=0;j<d_w;j++)
		{
			int sample[temp_size];
			for(int p=0;p<h_t;p++)
			{
				for(int q=0;q<w_t;q++)
				{
					//sample[p*w_t+q] = red[(i+p)*w_i + (j+q)];
					for(int k=0;k<c_t;k++)
						{
							sample[p*w_t*c_t + q*c_t + k] = img[(i+p)*w_i*c_t + (j+q)*c_t + k];
						}
				}
			}
			red_corr[i][j] = find_corr(sample, temp_arr, temp_size);
		}
	}

	puts("Correlation Coefficient calculated!");
	puts("Processing...\n");

	double max_col[d_w];
	int index_col[d_w];
	double max_temp;
	for(int j=0;j<d_w;j++)
	{
		index_col[j] = 0;
		max_temp =  red_corr[0][j];
		for(int i=1;i<d_h;i++)
		{
			if(red_corr[i][j] > max_temp)
			{
				max_temp = red_corr[i][j];
				index_col[j] = i;
			}
		}
		max_col[j] = max_temp;
	}

	double max_col_val = max_col[0];
	int max_col_id=index_col[0];

	for(int i=1;i<d_w; i++)
	{
		if(max_col[i] > max_col_val)
		{
			max_col_val = max_col[i];
			max_col_id = index_col[i];
		}
	}

	double max_row[d_h];
	int index_row[d_h];
	for(int i=0;i<d_h;i++)
	{
		index_row[i] = 0;
		max_temp = red_corr[i][0];
		for(int j=1;j<d_w;j++)
		{
			if(red_corr[i][j] > max_temp)
			{
				max_temp = red_corr[i][j];
				index_row[i] = j;
			}			
		}
		max_row[i] = max_temp;
	}

	double max_row_val=max_row[0];
	int max_row_id = index_row[0];
	for(int i=1;i<d_w;i++)
	{
		if(max_row[i] > max_row_val)
		{
			max_row_val= max_row[i];
			max_row_id = index_row[i];
		}
	}
	puts("Template was found at:");
	printf("Height pixel:\t%d\n",max_col_id);
	printf("Width pixel:\t%d\n",max_row_id);

	for(int i=0;i<h_i;i++)
	{
		for(int j=0;j<w_i;j++)
		{
			for(int k=0;k<3;k++)
			{
				out_img[i*w_i*3 + j*3 + k] = img[i*w_i*3 + j*3 + k];
			}
		}
	}

			int color = 0;
			
		{	for(int k=0;k<w_t;k++)

			{
				for(int c=0;c<c_i;c++)
				{
				out_img[max_col_id*w_i*c_i + (max_row_id+k)*c_i +c ] = color;
				out_img[(max_col_id+h_t)*w_i*c_i + (max_row_id+k)*c_i +c ] = color;
				}
			}

			for(int k=0;k<h_t;k++)

			{
				for(int c=0;c<c_i;c++)
				{
				out_img[(max_col_id+k)*w_i*c_i + (max_row_id)*c_i +c ] = color;
				out_img[(max_col_id+k)*w_i*c_i + (max_row_id+w_t)*c_i +c ] = color;
				}
			}
		}
	putchar('\n');
	puts("Saving result...");
	stbi_write_png(result_path, w_i, h_i,3, out_img, 0);
    stbi_image_free(img);
    stbi_image_free(out_img);
    stbi_image_free(temp);
    puts("Finish!!");
	return(0);
}