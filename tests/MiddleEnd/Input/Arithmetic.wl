int main() {
    int v1 = 1;
    int v2 = 2;
    int v3 = 3;
    int v4 = 4;
    int v5 = 5;
    v2 = v1 + 10; // 11
    v3 = v1 + v2; // 12
    v4 = v2 + v3; // 23
    v5 = v3 + v4; // 35
    return v5;
}