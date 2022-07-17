int main() {
    int v1 = 1;
    int v2 = 1;
    int v3 = 1;
    int v4 = 1;
    int v5 = 1;
    v2 = v1 &  1; // 1
    v3 = v1 & v2; // 1
    v4 = v2 ^ v3; // 0
    v5 = v3 & v4; // 0
    return v5;
}