__kernel void helloworld(__global double* in, __global double* out)
{
    //cout << "test" << endl;
    printf("test\n");
    int num = get_global_id(0);
    out[num] = in[num] / 2.4 * (in[num] / 6);
}
