
#include <cstring>
#include <fplll/fplll.h>

#ifndef TESTDATADIR
#define TESTDATADIR ".."
#endif
using namespace std;
using namespace fplll;

/**
@brief Test if CVP function returns correct vector.

@param A              input lattice
@param b              expected vector
@return
*/

template <class ZT>
int test_cvp(ZZ_mat<ZT> &A, vector<Z_NR<mpz_t>> &target, vector<Z_NR<mpz_t>> &b, const int method)
{
vector<Z_NR<mpz_t>> sol_coord;  // In the LLL-reduced basis
vector<Z_NR<mpz_t>> solution;
ZZ_mat<mpz_t> u;

// cerr << "A " << endl << A << endl;
int status =
   lll_reduction(A, u, LLL_DEF_DELTA, LLL_DEF_ETA, LM_WRAPPER, FT_DEFAULT, 0, LLL_DEFAULT);
if (status != RED_SUCCESS)
{
 cerr << "LLL reduction failed: " << get_red_status_str(status) << endl;
 return status;
}

status = closest_vector(A, target, sol_coord, method);

if (status != RED_SUCCESS)
{
 cerr << "Failure: " << get_red_status_str(status) << endl;
 return status;
}

vector_matrix_product(solution, sol_coord, A);

// cerr << "A red" << endl << A << endl;
// cerr << "sol_coord : " << sol_coord << endl;
// cerr << "solution : " << solution << endl;
// cerr << "expected : " << b << endl;
// cerr << "target : " << target << endl;

bool correct = true;
for (int i = 0; i < A.get_cols(); i++)
{
 correct = correct && (solution[i] == b[i]);
}
if (!correct)
 return 1;

return 0;
}

/**
@brief Test if CVP function returns the correct vector

@param input_filename_lattice   filename of an input lattice
@param input_filename_target    filename of a target vector
@param output_filename  filename of the expected vector
@return
*/

template <class ZT>
int test_filename(const char *input_filename_lattice, const char *input_filename_target,
               const char *output_filename, const int method = CVPM_FAST)
{
ZZ_mat<ZT> A;
int status = 0;
status |= read_file(A, input_filename_lattice);

vector<Z_NR<mpz_t>> t;
status |= read_file(t, input_filename_target);

vector<Z_NR<mpz_t>> b;
status |= read_file(b, output_filename);

status |= test_cvp<ZT>(A, t, b, method);
return status;
}

/**
@brief Run CVP tests.

@param argc             ignored
@param argv             ignored
@return
*/

int main(int argc, char *argv[])
{

int status = 0;
ZZ_mat<ZT> A;
status |= test_cvp<ZT>(A, t, b, method);

if (status == 0)
{
 cerr << "All tests passed." << endl;
 return 0;
}
else
{
 return -1;
}

return 0;
}