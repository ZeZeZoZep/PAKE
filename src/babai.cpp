#include "../include/babai.h"
int S_numbers[PARAM_K*PARAM_K]= {
    2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
    -1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0, -1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -1,  2,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, -1,  2,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0, -1,  2,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0, -1,  2,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, -1,  2,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0, -1,  2,  0,  0,  1,
    0,  0,  0,  0,  0,  0,  0,  0, -1,  2,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  2,  1,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  1
};
int g_numbers[PARAM_K]= { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

// Function to compute modular inverse
int modInverse(int a, int q) {
    a = a % q;
    for (int x = 1; x < q; x++) {
        if ((a * x) % q == 1) return x;
    }
    return -1; // No inverse found
}

// Compute c_j values
VectorXi compute_cj(const MatrixXi& V, const VectorXi& g, int q) {
    int k = V.cols();
    VectorXi c(k);

    int g_dot_g = g.dot(g) % q;
    long int g_dot_g_inv = modInverse(g_dot_g, q);
    //cout<<g_dot_g_inv<<endl;

    for (int j = 0; j < k; j++) {
        int c_j = (V.col(j).dot(g) * g_dot_g_inv)% q;
        if (c_j < 0) c_j += q; // Ensure non-negative
        c(j) = c_j;
        //cout<<V.col(j).dot(g)<<"  "<<V.col(j).dot(g) * g_dot_g_inv<<"  "<<c_j<<endl;
    }
    return c;
}

// Gram-Schmidt Orthogonalization
MatrixXd gram_schmidt(const MatrixXi& V) {
    int k = V.cols();
    MatrixXd GSO_matrix = V.cast<double>();

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < i; j++) {
            double projection = (GSO_matrix.col(j).dot(GSO_matrix.col(i))) /
                                (GSO_matrix.col(j).dot(GSO_matrix.col(j)));
            GSO_matrix.col(i) -= projection * GSO_matrix.col(j);
        }
    }
    return GSO_matrix;
}

// Babai’s Nearest Plane Algorithm
pair<int, VectorXi> babai_nearest_plane(const VectorXi& b) {
    //cout<< "vector b: " << b << endl;
    int q= PARAM_Q;
    // Define g vector
    VectorXi g =  Map<Vector<int, PARAM_K>>(g_numbers);

    // Define basis S_k
    //MatrixXi S_k(k, k);
    MatrixXi S_k = Map<Matrix<int, PARAM_K, PARAM_K>>(S_numbers);

    // Compute V = q * S_k^{-T}
    MatrixXd S_k_inv_T = S_k.cast<double>().inverse();
    MatrixXi V = (q * S_k_inv_T).cast<int>();

    //cout << "\nMatrice duale V = q S_k^{-T}:\n" << V << endl;

    // Compute Gram-Schmidt orthogonalization
    MatrixXd GS_V = gram_schmidt(V);
    int k = V.cols();
    VectorXd b_current = b.cast<double>();
    VectorXi rounded_l = VectorXi::Zero(k);

    VectorXi c = compute_cj(V, g, q);
    //cout << "\nVector c:\n" << c.transpose() << endl;
    // Step 2: Iterate backwards from k to 1
    for (int j = k - 1; j >= 0; j--) {
        VectorXd v_star_j = GS_V.col(j);
        double l_j = b_current.dot(v_star_j) / v_star_j.dot(v_star_j);
        rounded_l(j) = round(l_j); // Compute ⟦l_j⟧

        // Update b_{j-1} using column vectors
        b_current -= (l_j - rounded_l(j)) * v_star_j + rounded_l(j) * V.col(j).cast<double>();
    }

    // Step 3: Compute z and e
    int z = (rounded_l.dot(c)) % q;
    if (z < 0) z += q; // Ensure non-negative

    VectorXi gz = g * z;
    VectorXi e;
  /*
    VectorXi e = (b - gz).array();
    for(int i=0; i<k; i++) e(i) = e(i) % q;
    // Wrap-around correction
    for (int i = 0; i < k; i++) {
        if (e(i) > q / 4) e(i) -= q;
        else if (e(i) < -q / 4) e(i) += q;
    }
 */
    return {z, e};
}

/* // Main Function
int main() {
    int q = 1073734913;
    int k = 12;

    // Define g vector
    VectorXi g =  Map<Vector<int, PARAM_K>>(g_numbers);

    // Define basis S_k
    //MatrixXi S_k(k, k);
    MatrixXi S_k = Map<Matrix<int, PARAM_K, PARAM_K>>(S_numbers);

    // Compute V = q * S_k^{-T}
    MatrixXd S_k_inv_T = S_k.cast<double>().inverse();
    MatrixXi V = (q * S_k_inv_T).cast<int>();

    cout << "\nMatrice duale V = q S_k^{-T}:\n" << V << endl;

    // Compute Gram-Schmidt orthogonalization
    MatrixXd GS_V = gram_schmidt(V);

    cout << "\nMatrice ortonormalizzata con Gram-Schmidt:\n" << GS_V << endl;

    // Define b
    int s = 22;
    VectorXi e(k);
    e << 1, -1, 2, 2, -1, -2, 1, -2, 2, 3, -4, 1;

    VectorXi b = (g * s + e).array();
    for(int i=0; i<k; i++) b(i) = b(i) % q;

    cout << "\nVector b:\n" << b.transpose() << endl;

    // Apply Babai's nearest plane algorithm
    auto [z, e_result] = babai_nearest_plane(V, GS_V, b, g, q);

    cout << "\nValore di z: " << z << endl;
    cout << "Errore e:\n" << e_result.transpose() << endl;

    return 0;
} */
