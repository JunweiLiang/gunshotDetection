#include<iostream>
#include<fstream>
#include<string>
#include<stdio.h>
#include<sstream>
#include<cstdlib>


extern "C" {
#include <vl/generic.h>
#include <vl/homkermap.h>
}

using namespace std;

int main (int argc, const char * argv[]) {

	if( argc != 3 && argc != 4 )
	{
		fprintf(stdout, "Usage: cat original_feature | homo_map order gamma [kernel_tyle(0=chi2 1=interset 2=JS default=chi2)] > target_feature\n");
		return 1;
	}


	VlHomogeneousKernelType kernel_type=VlHomogeneousKernelChi2;

	if (argc==4)
	{
		int arg_kernel_type = atoi(argv[3]);

		if (arg_kernel_type == 0)
		{
			kernel_type=VlHomogeneousKernelChi2;
		}
		else if (arg_kernel_type == 1)
		{
			kernel_type=VlHomogeneousKernelIntersection;
		}
		else if (arg_kernel_type == 2)
		{
			kernel_type=VlHomogeneousKernelJS;
		}
		else
		{
			fprintf(stdout, "Kernel Type is wrong\n");
			return 1;
		}
	}

	double gamma=atof(argv[2]);
	const int order=atoi(argv[1]);
	const int dim_exp = order*2+1;

	double period=-1;
	double *psi = new double[dim_exp];
	vl_size psiStride=1;

	VlHomogeneousKernelMap * hom = vl_homogeneouskernelmap_new(
			kernel_type, gamma, order, period,
			VlHomogeneousKernelMapWindowRectangular) ;

	string line;
	while(getline(std::cin,line))
	{
		int acc=1;
		int idx=1;
		int pre_idx=1;
		double value=0;
		istringstream ss(line);
		bool ishead = true;
		while(!ss.eof())
		{
			string item;
			getline(ss, item, ' ');
			int ret = sscanf(item.c_str(), "%d:%lf", &idx, &value); 
			if( ret != 2 ){
				//fprintf(stderr, "sscanf ret = %d != 2, skipping\n", ret);
				continue;
			}

			acc += dim_exp*(idx - pre_idx);
			vl_homogeneouskernelmap_evaluate_d(hom, psi, psiStride, value);
			for(int i = 0; i < dim_exp; ++i)
			{
				if( psi[i] != 0 ){
					if (!ishead)
					{
						fprintf(stdout, " ");
					}
					ishead = false;
					fprintf(stdout, "%d:%g", acc + i, psi[i]);
				}
				//cout << acc + i << ":" << psi[i];
			}
			pre_idx = idx;
		}
		fprintf(stdout, "\n");
	}

	delete[] psi;
	vl_homogeneouskernelmap_delete(hom);
	return 0;
}
