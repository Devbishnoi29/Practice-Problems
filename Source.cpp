#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
int N, cnt, qcnt;
int corner[65][65];
typedef struct node {
	int data;
	node * next;
}node;
typedef struct cell {
	int x, y, dir;
}cell;
typedef struct qnode {
	int data;
	int dist;
	qnode * next;
}qnode;
node ** root;
node * pool;
qnode * qpool;
cell cs[1001];
int rev[] = { 2, 3, 0, 1 };
int movx[] = { 0, 1, 0, -1 };
int movy[] = { 1, 0, -1, 0 };
void initl(int n) {
	N = n;
	cnt = 0;
	qcnt = 0;
	for (int i = 0; i < 65; i++)
		for (int j = 0; j < 65; j++)
			corner[i][j] = 0;
	int sz = n * n * 4 + 3;
	root = new node *[sz];
	pool = new node[sz * 4];
	qpool = new qnode[sz * 4];
	for (int i = 0; i < sz; i++) {
		root[i] = NULL;
	}
	return;
}
int getidx(int x, int y, int dir) {
	return dir * N * N + x * N + y;
}
void addingEdge(int x1, int y1, int d1, int x2, int y2, int d2) {
	int src = getidx(x1, y1, d1);
	int des = getidx(x2, y2, d2);
	node * temp = root[src];
	while (temp) {
		if (temp->data == des)
			return;
		temp = temp->next;
	}
	//cout << src << "-->" << des << endl;
	temp = &pool[cnt++];
	temp->data = des;
	temp->next = root[src];
	root[src] = temp;
	return;
}
void addEdge(int x1, int y1, int d1, int x2, int y2, int d2) {
	//cout << x1 << " " << y1 << " " << d1 << " to " << x2 << " " << y2 << " " << d2 << endl;
	if (corner[x2][y2]) {
		for (int i = 0; i < 4; i++) {
			addingEdge(x1, y1, d1, x2, y2, i);
		}
		return;
	}
	addingEdge(x1, y1, d1, x2, y2, d2);
}
void addBuilding(int id, int locX, int locY, int w, int h, int px, int py) {
	h += 2;
	w += 2;
	int up = locY - 1;
	int left = locX - 1;
	int down = up + h - 1;
	int right = left + w - 1;
	px++;
	py++;
	int pkx = up + py;
	int pky = left + px;
	px = pkx;
	py = pky;
	corner[up][left] = 1;
	corner[up][right] = 1;
	corner[down][left] = 1;
	corner[down][right] = 1;
	for (int i = left; i < right; i++)
		addEdge(up, i, 0, up, i + 1, 0);
	for (int i = left; i < right; i++)
		addEdge(down, i + 1, 2, down, i, 2);
	for (int i = up; i < down; i++)
		addEdge(i, right, 1, i + 1, right, 1);
	for (int i = up; i < down; i++)
		addEdge(i + 1, left, 3, i, left, 3);

	for (int i = 0; i < 4; i++) {
		int xx = up + movx[i];
		int yy = left + movy[i];
		if (xx >= 0 && xx < N && yy >= 0 && yy < N) {
			int indexx = getidx(xx, yy, rev[i]);
			if (root[indexx])
				addEdge(xx, yy, rev[i], up, left, 0);
		}
	}

	for (int i = 0; i < 4; i++) {
		int xx = up + movx[i];
		int yy = right + movy[i];
		if (xx >= 0 && xx < N && yy >= 0 && yy < N) {
			int indexx = getidx(xx, yy, rev[i]);
			if (root[indexx])
				addEdge(xx, yy, rev[i], up, right, 0);
		}
	}
	for (int i = 0; i < 4; i++) {
		int xx = down + movx[i];
		int yy = left + movy[i];
		if (xx >= 0 && xx < N && yy >= 0 && yy < N) {
			int indexx = getidx(xx, yy, rev[i]);
			if (root[indexx])
				addEdge(xx, yy, rev[i], down, left, 0);
		}
	}
	for (int i = 0; i < 4; i++) {
		int xx = down + movx[i];
		int yy = right + movy[i];
		if (xx >= 0 && xx < N && yy >= 0 && yy < N) {
			int indexx = getidx(xx, yy, rev[i]);
			if (root[indexx])
				addEdge(xx, yy, rev[i], down, right, 0);
		}
	}

	if (left + 1 == py) {
		addEdge(px, left, 3, px, py, 0);
		addEdge(px, py, 2, px, left, 3);
		cs[id].dir = 2;
	}
	if (up + 1 == px) {
		addEdge(up, py, 0, px, py, 1);
		addEdge(px, py, 3, up, py, 0);
		cs[id].dir = 3;
	}
	if (right - 1 == py) {
		addEdge(px, py, 0, px, right, 1);
		addEdge(px, right, 1, px, py, 2);
		cs[id].dir = 0;
	}
	if (px == down - 1) {
		addEdge(px, py, 1, down, py, 2);
		addEdge(down, py, 2, px, py, 3);
		cs[id].dir = 1;
	}
	cs[id].x = px;
	cs[id].y = py;
	return;
}
void push(qnode ** front, qnode ** rear, int src, int d) {
	qnode * temp = &qpool[qcnt++];
	temp->data = src;
	temp->dist = d;
	temp->next = NULL;
	if ((*front) == NULL) {
		(*front) = temp;
		(*rear) = temp;
		//cout << "data pushed : " << (*rear)->data << endl;
		return;
	}
	(*rear)->next = temp;
	(*rear) = temp;
	//cout << "data pushed : " << (*rear)->data << endl;
	return;
}
void pop(qnode ** front, qnode ** rear, int &data, int &d) {
	if ((*front) == NULL) {
		//cout << "queue empty" << endl;
		return;
	}
	data = (*front)->data;
	d = (*front)->dist;
	(*front) = (*front)->next;
	//cout << "popped : " << data << endl;
	return;
}
int bfs(int src, int dest){
	qnode * front, *rear;
	front = NULL;
	rear = NULL;
	int *visit = new int[N * N * 4];
	for (int i = 0; i < N * N * 4; i++) {
		visit[i] = 0;
	}
	push(&front, &rear, src, 0);
	visit[src] = 1;
	while (true) {
		int curr = -1, d = -1;
		pop(&front, &rear, curr, d);
		if (curr == dest)
			return d;
		node * temp = root[curr];
		while (temp) {
			if (visit[temp->data] == 0) {
				push(&front, &rear, temp->data, d + 1);
				visit[temp->data] = 1;
			}
			temp = temp->next;
		}
	}
}
int getDistance(int id1, int id2) {
	int src = getidx(cs[id1].x, cs[id1].y, cs[id1].dir);
	int dest = getidx(cs[id2].x, cs[id2].y, rev[cs[id2].dir]);
	int distance  = bfs(src, dest);
	return distance;
}
int main() {
	freopen("Text.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	int t;
	cin >> t;
	while (t--) {
		int n, cmd;
		cin >> cmd >> n;
		initl(n);
		while (true) {
			cin >> cmd;
			if (cmd == 1) {
				int id, locX, locY, w, h, px, py;
				cin >> id >> locX >> locY >> w >> h >> px >> py;
				addBuilding(id, locX, locY, w, h, px, py);
			}
			else if (cmd == 2) {
				int id1, id2, dis, d;
				cin >> id1 >> id2 >> dis;
				d = getDistance(id1, id2);
				if (dis == d)
					cout << "Correct" << endl;
				else
					cout << "Wrong " << dis << " : " << d << endl;
			}
			else
				break;
		}
	}
	return 0;
}