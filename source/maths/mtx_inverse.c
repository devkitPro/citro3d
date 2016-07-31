#include <c3d/maths.h>

void Mtx_Inverse(C3D_Mtx* out)
{
	float inv[16], det;
    int i;

    inv[0] = out->m[5]  * out->m[10] * out->m[15] - 
             out->m[5]  * out->m[11] * out->m[14] - 
             out->m[9]  * out->m[6]  * out->m[15] + 
             out->m[9]  * out->m[7]  * out->m[14] +
             out->m[13] * out->m[6]  * out->m[11] - 
             out->m[13] * out->m[7]  * out->m[10];

    inv[4] = -out->m[4]  * out->m[10] * out->m[15] + 
              out->m[4]  * out->m[11] * out->m[14] + 
              out->m[8]  * out->m[6]  * out->m[15] - 
              out->m[8]  * out->m[7]  * out->m[14] - 
              out->m[12] * out->m[6]  * out->m[11] + 
              out->m[12] * out->m[7]  * out->m[10];

    inv[8] = out->m[4]  * out->m[9] * out->m[15] - 
             out->m[4]  * out->m[11] * out->m[13] - 
             out->m[8]  * out->m[5] * out->m[15] + 
             out->m[8]  * out->m[7] * out->m[13] + 
             out->m[12] * out->m[5] * out->m[11] - 
             out->m[12] * out->m[7] * out->m[9];

    inv[12] = -out->m[4]  * out->m[9] * out->m[14] + 
               out->m[4]  * out->m[10] * out->m[13] +
               out->m[8]  * out->m[5] * out->m[14] - 
               out->m[8]  * out->m[6] * out->m[13] - 
               out->m[12] * out->m[5] * out->m[10] + 
               out->m[12] * out->m[6] * out->m[9];
    
    det = out->m[0] * inv[0] + out->m[1] * inv[4] + out->m[2] * inv[8] + out->m[3] * inv[12];
    if (det != 0.0f){
	
		inv[1] = -out->m[1]  * out->m[10] * out->m[15] + 
				  out->m[1]  * out->m[11] * out->m[14] + 
				  out->m[9]  * out->m[2] * out->m[15] - 
				  out->m[9]  * out->m[3] * out->m[14] - 
				  out->m[13] * out->m[2] * out->m[11] + 
				  out->m[13] * out->m[3] * out->m[10];
	
		inv[5] = out->m[0]  * out->m[10] * out->m[15] - 
				 out->m[0]  * out->m[11] * out->m[14] - 
				 out->m[8]  * out->m[2] * out->m[15] + 
				 out->m[8]  * out->m[3] * out->m[14] + 
				 out->m[12] * out->m[2] * out->m[11] - 
				 out->m[12] * out->m[3] * out->m[10];
	
		inv[9] = -out->m[0]  * out->m[9] * out->m[15] + 
				  out->m[0]  * out->m[11] * out->m[13] + 
				  out->m[8]  * out->m[1] * out->m[15] - 
				  out->m[8]  * out->m[3] * out->m[13] - 
				  out->m[12] * out->m[1] * out->m[11] + 
				  out->m[12] * out->m[3] * out->m[9];
	
		inv[13] = out->m[0]  * out->m[9] * out->m[14] - 
				  out->m[0]  * out->m[10] * out->m[13] - 
				  out->m[8]  * out->m[1] * out->m[14] + 
				  out->m[8]  * out->m[2] * out->m[13] + 
				  out->m[12] * out->m[1] * out->m[10] - 
				  out->m[12] * out->m[2] * out->m[9];
	
		inv[2] = out->m[1]  * out->m[6] * out->m[15] - 
				 out->m[1]  * out->m[7] * out->m[14] - 
				 out->m[5]  * out->m[2] * out->m[15] + 
				 out->m[5]  * out->m[3] * out->m[14] + 
				 out->m[13] * out->m[2] * out->m[7] - 
				 out->m[13] * out->m[3] * out->m[6];
	
		inv[6] = -out->m[0]  * out->m[6] * out->m[15] + 
				  out->m[0]  * out->m[7] * out->m[14] + 
				  out->m[4]  * out->m[2] * out->m[15] - 
				  out->m[4]  * out->m[3] * out->m[14] - 
				  out->m[12] * out->m[2] * out->m[7] + 
				  out->m[12] * out->m[3] * out->m[6];
	
		inv[10] = out->m[0]  * out->m[5] * out->m[15] - 
				  out->m[0]  * out->m[7] * out->m[13] - 
				  out->m[4]  * out->m[1] * out->m[15] + 
				  out->m[4]  * out->m[3] * out->m[13] + 
				  out->m[12] * out->m[1] * out->m[7] - 
				  out->m[12] * out->m[3] * out->m[5];
	
		inv[14] = -out->m[0]  * out->m[5] * out->m[14] + 
				   out->m[0]  * out->m[6] * out->m[13] + 
				   out->m[4]  * out->m[1] * out->m[14] - 
				   out->m[4]  * out->m[2] * out->m[13] - 
				   out->m[12] * out->m[1] * out->m[6] + 
				   out->m[12] * out->m[2] * out->m[5];
	
		inv[3] = -out->m[1] * out->m[6] * out->m[11] + 
				  out->m[1] * out->m[7] * out->m[10] + 
				  out->m[5] * out->m[2] * out->m[11] - 
				  out->m[5] * out->m[3] * out->m[10] - 
				  out->m[9] * out->m[2] * out->m[7] + 
				  out->m[9] * out->m[3] * out->m[6];
	
		inv[7] = out->m[0] * out->m[6] * out->m[11] - 
				 out->m[0] * out->m[7] * out->m[10] - 
				 out->m[4] * out->m[2] * out->m[11] + 
				 out->m[4] * out->m[3] * out->m[10] + 
				 out->m[8] * out->m[2] * out->m[7] - 
				 out->m[8] * out->m[3] * out->m[6];
	
		inv[11] = -out->m[0] * out->m[5] * out->m[11] + 
				   out->m[0] * out->m[7] * out->m[9] + 
				   out->m[4] * out->m[1] * out->m[11] - 
				   out->m[4] * out->m[3] * out->m[9] - 
				   out->m[8] * out->m[1] * out->m[7] + 
				   out->m[8] * out->m[3] * out->m[5];
	
		inv[15] = out->m[0] * out->m[5] * out->m[10] - 
				  out->m[0] * out->m[6] * out->m[9] - 
				  out->m[4] * out->m[1] * out->m[10] + 
				  out->m[4] * out->m[2] * out->m[9] + 
				  out->m[8] * out->m[1] * out->m[6] - 
				  out->m[8] * out->m[2] * out->m[5];
	
		det = 1.0 / det;
	
		for (i = 0; i < 16; i++)
			out->m[i] = inv[i] * det;
    }

}
