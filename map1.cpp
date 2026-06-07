#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <stack>

using namespace std;

// Daftar Instruksi:
// 0 = Maju
// 1 = Belok Kiri
// 2 = Belok Kanan
// 3 = Panggil F1
// 4 = Panggil F2

// Arah: 0=Atas(Utara), 1=Kanan(Timur), 2=Bawah(Selatan), 3=Kiri(Barat)
int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};

// Struktur State Game 
struct GameState {
    int x, y;
    int dir;
    int orbs_collected;
    int total_orbs;
    vector<vector<int>> grid;
    int step_limit; // Pengaman Anti Infinite Loop
};

// Fungsi findPortal: untuk menentukkan dimana pasangan portal teleportasi
pair<int, int> findPortalExit(const vector<vector<int>>& grid) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            if (grid[i][j] == 5) return {i, j};
        }
    }
    return {-1, -1};
}

// Fungsi Simulator, akan mengembalikan nilai 'true' jika kombinasi instruksi berhasil
bool runSimulator(int func_id, const vector<int>& f1, const vector<int>& f2, GameState& state) {
    const vector<int>& current_func = (func_id == 1) ? f1 : f2;

    for (int cmd : current_func) {
        if (state.step_limit <= 0) return false; 
        state.step_limit--;

        if (cmd == 0) { // MAJU
            int nx = state.x + dx[state.dir];
            int ny = state.y + dy[state.dir];

            if (nx < 0 || nx >= state.grid.size() || ny < 0 || ny >= state.grid[0].size() || state.grid[nx][ny] == 1) {
                return false; 
            }

            state.x = nx;
            state.y = ny;

            if (state.grid[state.x][state.y] == 4) {
                pair<int, int> exitPortal = findPortalExit(state.grid);
                state.x = exitPortal.first;
                state.y = exitPortal.second;
            }

            if (state.grid[state.x][state.y] == 8) {
                state.orbs_collected++;
                state.grid[state.x][state.y] = 0; 
            }

            if (state.grid[state.x][state.y] == 3) {
                if (state.orbs_collected == state.total_orbs) {
                    return true; 
                }
            }

        } else if (cmd == 1) { 
            state.dir = (state.dir + 3) % 4; 
        } else if (cmd == 2) { 
            state.dir = (state.dir + 1) % 4; 
        } else if (cmd == 3) { 
            if (runSimulator(1, f1, f2, state)) return true;
        } else if (cmd == 4) { 
            if (runSimulator(2, f1, f2, state)) return true;
        }
    }
    
    return false; 
}

void solveBFS(GameState initialState, int max_f1_limit) {
    queue<vector<int>> q;
    q.push({}); 
    int state_dievaluasi = 0;

    cout << "===|BFS|===" << endl;
   
    auto start = chrono::high_resolution_clock::now();

    while (!q.empty()) {
        vector<int> current_f1 = q.front();
        q.pop();
        state_dievaluasi++;

        GameState state = initialState;

        if (runSimulator(1, current_f1, {}, state)) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            cout << "SOLUSI DITEMUKAN!" << endl;
            cout << "Waktu Eksekusi : " << duration.count() << " microseconds" << endl;
            cout << "Panjang Instruksi : " << current_f1.size() << " langkah" << endl;
            cout << "Total Operasi/State Dievaluasi: " << state_dievaluasi << endl;
            cout << "Urutan (0=Maju, 1=Kiri, 2=Kanan): ";
            for (int cmd : current_f1) cout << "[" << cmd << "] ";
            cout << "\n\n";
            return;
        }
        if (current_f1.size() < max_f1_limit) {
            for (int i = 0; i <= 2; i++) { 
                vector<int> next_f1 = current_f1;
                next_f1.push_back(i);
                q.push(next_f1);
            }
        }
    }

    cout << "  [BFS] Gagal menemukan solusi dalam batas " << max_f1_limit << " langkah.\n";
}
//Algoritma Backtracking
void solveBacktracking(GameState initialState, int max_f1_limit) {
    stack<vector<int>> s;
    s.push({}); 
    int state_dievaluasi = 0;

    cout << "===|BACKTRACKING|===" << endl;
    auto start = chrono::high_resolution_clock::now();

    while (!s.empty()) {
        vector<int> current_f1 = s.top();
        s.pop();
        state_dievaluasi++;

        GameState state = initialState;

        if (runSimulator(1, current_f1, {}, state)) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            cout << "SOLUSI DITEMUKAN!" << endl;
            cout << "Waktu Eksekusi : " << duration.count() << " microseconds" << endl;
            cout << "Panjang Instruksi : " << current_f1.size() << " langkah" << endl;
            cout << "Total Operasi/State Dievaluasi: " << state_dievaluasi << endl;
            cout << "Urutan : ";
            for (int cmd : current_f1) cout << "[" << cmd << "] ";
            cout << "\n\n";
            return;
        }

        if (current_f1.size() < max_f1_limit) {
            for (int i = 2; i >= 0; i--) { 
                vector<int> next_f1 = current_f1;
                next_f1.push_back(i);
                s.push(next_f1);
            }
        }
    }
    cout << "Gagal menemukan solusi.\n";
}

// Algoritma Dijkstra menggunakan priority queue
struct CompareCost {
    bool operator()(const vector<int>& a, const vector<int>& b) {
        return a.size() > b.size(); 
    }
};

void solveDijkstra(GameState initialState, int max_f1_limit) {
    priority_queue<vector<int>, vector<vector<int>>, CompareCost> pq;
    pq.push({});
    int state_dievaluasi = 0;

    cout << "===|DIJKSTRA|===" << endl;
    auto start = chrono::high_resolution_clock::now();

    while (!pq.empty()) {
        vector<int> current_f1 = pq.top();
        pq.pop();
        state_dievaluasi++;

        GameState state = initialState;

        if (runSimulator(1, current_f1, {}, state)) {
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            cout << "SOLUSI DITEMUKAN!" << endl;
            cout << "Waktu Eksekusi : " << duration.count() << " microseconds" << endl;
            cout << "Panjang Instruksi : " << current_f1.size() << " langkah" << endl;
            cout << "Total Operasi/State Dievaluasi: " << state_dievaluasi << endl;
            cout << "Urutan : ";
            for (int cmd : current_f1) cout << "[" << cmd << "] ";
            cout << "\n\n";
            return;
        }

        if (current_f1.size() < max_f1_limit) {
            for (int i = 0; i <= 2; i++) { 
                vector<int> next_f1 = current_f1;
                next_f1.push_back(i);
                pq.push(next_f1);
            }
        }
    }
    cout << "Gagal menemukan solusi.\n";
}

int main() {
    // Definisi Map 1
    vector<vector<int>> map1 = {
        {2, 0, 0, 8}, // Start, Jalan, Jalan, Bola
        {0, 0, 0, 8}, // Jalan, Jalan, Jalan, Bola
        {0, 0, 0, 8}, // Jalan, Jalan, Jalan, Bola
        {0, 0, 0, 3}  // Jalan, Jalan, Jalan, Goal 
    };

    GameState initialState;
    initialState.x = 0;
    initialState.y = 0;
    initialState.dir = 1; 
    initialState.orbs_collected = 0;
    initialState.total_orbs = 3;
    initialState.grid = map1;
    initialState.step_limit = 50; 

    cout << "=== EKSPERIMEN MAP 1: THE BASICS (Max 8 Langkah) ===" << endl;
    
    solveBFS(initialState, 8);
    solveBacktracking(initialState, 8);
    solveDijkstra(initialState, 8);

    return 0;
}