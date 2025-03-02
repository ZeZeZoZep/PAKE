import numpy as np
from fpylll import IntegerMatrix, GSO
import numpy as np

def compute_cj(V, g, q):
    """ Compute c_j such that v_j = c_j * g mod q using column vectors. """
    k = V.shape[1]  # Number of columns (correct dimension for v_j)
    c = np.zeros(k, dtype=int)

    # Compute the modular inverse of (g · g) mod q
    g_dot_g = np.dot(g, g) % q
    g_dot_g_inv = pow(int(g_dot_g), -1, q)  # Ensure g_dot_g is a Python int
    print( g_dot_g_inv)
    # Compute each c_j ensuring integer values (iterate over columns)
    for j in range(k):
        v_j = V[:, j]  # Take the column vector
        c_j = (np.dot(v_j, g) * g_dot_g_inv) % q
        c[j] = int(c_j)
        print(np.dot(v_j, g),np.dot(v_j, g) * g_dot_g_inv, c_j)
    return c



def gram_schmidt(V):
    """ Compute the Gram-Schmidt orthogonalization of V. """
    k = V.shape[1]
    GSO_matrix = np.zeros_like(V, dtype=float)
    basis = V.astype(float)

    for i in range(k):
        GSO_matrix[:, i] = basis[:, i]
        for j in range(i):
            projection = np.dot(GSO_matrix[:, j], basis[:, i]) / np.dot(GSO_matrix[:, j], GSO_matrix[:, j])
            GSO_matrix[:, i] -= projection * GSO_matrix[:, j]
    
    return GSO_matrix


def babai_nearest_plane(V, GS_V, b, g, q):
    """ Babai's Nearest Plane Algorithm using the correct column vectors of V. """
    k = V.shape[1]  # Ensure we iterate over columns of V
    b_current = b.astype(float)  # Ensure b is float for precision
    rounded_l = np.zeros(k, dtype=int)

    # Compute c_j correctly using column vectors
    c = compute_cj(V, g, q)

    print("\nChecking c_j values:")
    for i in range(k):
        print(f"c[{i}] * g % q = {g * c[i] % q} should match V[:,{i}]: {V[:,i]}")
    print("\nComputed c values:", c)

    # Step 2: Iterate backwards from k to 1
    for j in range(k - 1, -1, -1):
        v_star_j = GS_V[:, j]  # Take column from GS_V
        l_j = np.dot(b_current, v_star_j) / np.dot(v_star_j, v_star_j)
        rounded_l[j] = round(l_j)  # Compute ⟦l_j⟧
        print(f"l[{j}] = {l_j}, rounded_l[{j}] = {rounded_l[j]}")

        # Update b_{j-1} using column vectors
        b_current -= ((l_j - rounded_l[j]) * v_star_j + rounded_l[j] * V[:, j]).astype(float)

    print("\nFinal rounded_l values:", rounded_l)

    # Step 3: Compute z and e
    z = np.sum(rounded_l * c) % q
    if z < 0:
        z += q  # Ensure non-negative
    
    gz = g * z
    e = (b - gz).astype(int)  # Ensure e is an integer vector
    for i in range(k):
        if e[i] > q/4:
            e[i] -= q  # Ensure non-negative
    return z, e


def main():
    q = 3329
    k = 12  # Dimension of the problem

    # Define primitive vector g
    g = np.array([1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048])

    # Define basis V (for simplicity, taking q * inverse of S_k^T)
    S_k = np.array([
        [2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1],
        [-1, 2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0, -1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0, -1,  2,  0,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0, -1,  2,  0,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0, -1,  2,  0,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0, -1,  2,  0,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0, -1,  2,  0,  0,  0,  0],
        [0,  0,  0,  0,  0,  0,  0, -1,  2,  0,  0,  1],
        [0,  0,  0,  0,  0,  0,  0,  0, -1,  2,  0,  0],
        [0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  2,  1],
        [0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  1]
    ])

    # Compute V = q * S_k^{-T}
    S_k_inv_T = np.linalg.inv(S_k.T)
    V = (q * S_k_inv_T).astype(int)

    print("\nMatrice duale V = q S_k^{-T}:")
    print(V)

    # Compute Gram-Schmidt orthogonalization
    GS_V = gram_schmidt(V)

    print("\nMatrice ortonormalizzata con Gram-Schmidt:")
    print(GS_V)
    
    # Define b
    s= 10
    e = np.array([1, -1, 2, 2, -1, -2, 1, -2, 2, 3, -4, 1, 2])
    b = np.array([1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048])
    for i in range(0,k):
        b[i]=g[i]*s + e[i] % q
    print(b)
    # Apply Babai's nearest plane algorithm
    z, e = babai_nearest_plane(V, GS_V, b, g, q)

    print("\nValore di z:", z)
    print("Errore e:", e)

if __name__ == "__main__":
    main()
