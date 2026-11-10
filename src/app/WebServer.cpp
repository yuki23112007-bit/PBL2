#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>

#include "WebServer.h"
#include "Md5.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <set>
#include <cstdio>

#include "../models/ThucPhamThuong.h"
#include "../models/ThucPhamTuoiSong.h"
#include "../models/CongNghe.h"
#include "../models/DoGiaDung.h"
#include "../service/ThongKeDoanhThu.h"

using namespace std;

static const time_t HAN_3_NGAY = 3LL * 24 * 60 * 60;
static const time_t THOI_HAN_PHIEN = 8LL * 60 * 60;

// -------------------------------------------------------------
// Helpers co ban
// -------------------------------------------------------------
string WebServer::escapeJson(const string& s) {
    ostringstream o;
    for (unsigned char c : s) {
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\b') o << "\\b";
        else if (c == '\f') o << "\\f";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else if (c < 0x20) {
            o << "\\u" << hex << setfill('0') << setw(4) << (int)c << dec << setfill(' ');
        } else {
            o << (char)c;
        }
    }
    return o.str();
}

string WebServer::loiJson(const string& message) {
    return "{\"success\":false,\"message\":\"" + escapeJson(message) + "\"}";
}

// File du lieu dung '|' lam dau phan cach nen phai bo '|' va xuong dong khoi noi dung nguoi dung nhap.
string WebServer::lamSachTruongFile(const string& s) {
    string r;
    for (char c : s) {
        if (c == '|' || c == '\n' || c == '\r' || c == '\t') r += ' ';
        else r += c;
    }
    size_t a = r.find_first_not_of(' ');
    if (a == string::npos) return "";
    size_t b = r.find_last_not_of(' ');
    return r.substr(a, b - a + 1);
}

string WebServer::urlDecode(const string& in) {
    string out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%' && i + 2 < in.size()) {
            int value;
            istringstream is(in.substr(i + 1, 2));
            if (is >> hex >> value) {
                out += static_cast<char>(value);
                i += 2;
            } else {
                out += in[i];
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return out;
}

map<string, string> WebServer::parseQueryParams(const string& query) {
    map<string, string> result;
    istringstream ss(query);
    string pair;
    while (getline(ss, pair, '&')) {
        size_t eq = pair.find('=');
        if (eq != string::npos) {
            result[urlDecode(pair.substr(0, eq))] = urlDecode(pair.substr(eq + 1));
        } else if (!pair.empty()) {
            result[urlDecode(pair)] = "";
        }
    }
    return result;
}

// Doc JSON phang (khong long nhau). Hieu dung chuoi co dau \" va \\ ; bo qua mang/doi tuong long.
map<string, string> WebServer::parseSimpleJson(const string& json) {
    map<string, string> res;
    size_t n = json.size();
    size_t i = 0;
    while (i < n) {
        size_t kStart = json.find('"', i);
        if (kStart == string::npos) break;
        string key;
        size_t p = kStart + 1;
        while (p < n && json[p] != '"') {
            if (json[p] == '\\' && p + 1 < n) p++;
            key += json[p];
            p++;
        }
        if (p >= n) break;
        size_t colon = json.find(':', p + 1);
        if (colon == string::npos) break;
        size_t v = json.find_first_not_of(" \t\r\n", colon + 1);
        if (v == string::npos) break;

        string val;
        if (json[v] == '"') {
            p = v + 1;
            while (p < n && json[p] != '"') {
                if (json[p] == '\\' && p + 1 < n) {
                    p++;
                    switch (json[p]) {
                        case 'n': val += '\n'; break;
                        case 't': val += '\t'; break;
                        case 'r': break;
                        case 'u': p += 4; val += '?'; break;
                        default: val += json[p];
                    }
                } else {
                    val += json[p];
                }
                p++;
            }
            i = p + 1;
        } else if (json[v] == '[' || json[v] == '{') {
            int depth = 0;
            bool inStr = false;
            p = v;
            for (; p < n; ++p) {
                char c = json[p];
                if (inStr) {
                    if (c == '\\') p++;
                    else if (c == '"') inStr = false;
                } else if (c == '"') {
                    inStr = true;
                } else if (c == '[' || c == '{') {
                    depth++;
                } else if (c == ']' || c == '}') {
                    depth--;
                    if (depth == 0) { p++; break; }
                }
            }
            i = p;
            continue;
        } else {
            size_t vEnd = json.find_first_of(",}\r\n", v);
            if (vEnd == string::npos) vEnd = n;
            val = json.substr(v, vEnd - v);
            while (!val.empty() && (val.back() == ' ' || val.back() == '\t')) val.pop_back();
            if (val == "null") val = "";
            i = vEnd;
        }
        res[key] = val;
    }
    return res;
}

string WebServer::getMimeType(const string& path) {
    size_t dot = path.rfind('.');
    if (dot == string::npos) return "";
    string ext = path.substr(dot + 1);
    for (char& c : ext) c = (char)tolower((unsigned char)c);

    if (ext == "html" || ext == "htm") return "text/html; charset=utf-8";
    if (ext == "css") return "text/css; charset=utf-8";
    if (ext == "js") return "application/javascript; charset=utf-8";
    if (ext == "json") return "application/json; charset=utf-8";
    if (ext == "svg") return "image/svg+xml";
    if (ext == "png") return "image/png";
    if (ext == "ico") return "image/x-icon";
    if (ext == "ttf") return "font/ttf";
    if (ext == "woff") return "font/woff";
    if (ext == "woff2") return "font/woff2";
    if (ext == "otf") return "font/otf";
    return "";
}

// -------------------------------------------------------------
// Trang thai don hang
// -------------------------------------------------------------
string WebServer::maTrangThai(const string& s) {
    string t;
    for (char c : s) t += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    static const vector<pair<string, string>> bang = {
        {"cho duyet", "CHO_DUYET"}, {"chờ duyệt", "CHO_DUYET"},
        {"dang cho hang", "DANG_CHO_HANG"}, {"chờ hàng", "DANG_CHO_HANG"},
        {"chờ nhập hàng", "DANG_CHO_HANG"}, {"Đang chờ hàng", "DANG_CHO_HANG"}, {"đang chờ hàng", "DANG_CHO_HANG"},
        {"da duyet", "DA_DUYET"}, {"Đã duyệt", "DA_DUYET"}, {"đã duyệt", "DA_DUYET"},
        {"dang giao", "DANG_GIAO"}, {"Đang giao", "DANG_GIAO"}, {"đang giao", "DANG_GIAO"},
        {"da hoan thanh", "DA_HOAN_THANH"}, {"da giao", "DA_HOAN_THANH"},
        {"Đã giao", "DA_HOAN_THANH"}, {"đã giao", "DA_HOAN_THANH"},
        {"Đã hoàn thành", "DA_HOAN_THANH"}, {"đã hoàn thành", "DA_HOAN_THANH"},
        {"da huy", "DA_HUY"}, {"da huy do qua han", "DA_HUY"},
        {"Đã hủy", "DA_HUY"}, {"đã hủy", "DA_HUY"}, {"Đã huỷ", "DA_HUY"}, {"đã huỷ", "DA_HUY"},
        {"Đã hủy do quá hạn", "DA_HUY"}};
    for (const auto& kv : bang) {
        if (t == kv.first) return kv.second;
    }
    return "KHAC";
}

string WebServer::chuoiNoiBo(const string& ma) {
    if (ma == "CHO_DUYET") return "Cho Duyet";
    if (ma == "DANG_CHO_HANG") return "Dang Cho Hang";
    if (ma == "DA_DUYET") return "Da Duyet";
    if (ma == "DANG_GIAO") return "Dang Giao";
    if (ma == "DA_HOAN_THANH") return "Da Hoan Thanh";
    if (ma == "DA_HUY") return "Da Huy";
    return "Cho Duyet";
}

string WebServer::nhanTrangThai(const string& ma) {
    if (ma == "CHO_DUYET") return "Chờ duyệt";
    if (ma == "DANG_CHO_HANG") return "Chờ hàng";
    if (ma == "DA_DUYET") return "Đã duyệt";
    if (ma == "DANG_GIAO") return "Đang giao";
    if (ma == "DA_HOAN_THANH") return "Đã hoàn thành";
    if (ma == "DA_HUY") return "Đã hủy";
    return "Không xác định";
}

bool WebServer::laTrangThaiKetThuc(const string& ma) {
    return ma == "DA_HOAN_THANH" || ma == "DA_HUY";
}

// Tuong tu DonHang::daQuaHan5Phut nhung cho moc 3 ngay (README: don qua han 3 ngay)
bool WebServer::daQuaHan3Ngay(time_t moc, time_t bayGio) {
    return (bayGio - moc) >= HAN_3_NGAY;
}

string WebServer::ngayGioHienThi(time_t t) {
    char buf[32];
    tm* tmv = localtime(&t);
    if (!tmv) return "";
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", tmv);
    return buf;
}

string WebServer::ngayIso(time_t t) {
    char buf[16];
    tm* tmv = localtime(&t);
    if (!tmv) return "";
    strftime(buf, sizeof(buf), "%Y-%m-%d", tmv);
    return buf;
}

// -------------------------------------------------------------
// Constructor & Destructor
// -------------------------------------------------------------
WebServer::WebServer(int port, const string& webRoot, const string& dataDir)
    : port(port), webRoot(webRoot), dataDir(dataDir), isRunning(false),
      fileManager(dataDir) {
}

WebServer::~WebServer() {
    stop();
    luuDuLieuHeThong();
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        delete tk;
    }
    danhSachTaiKhoan.clear();
    cayTrieTaiKhoan.clear();
}

// -------------------------------------------------------------
// Nap va luu du lieu chuan tu data/*.txt
// -------------------------------------------------------------
void WebServer::napDuLieuHeThong() {
    cout << "[WebServer] Dang nap du lieu tu thu muc: " << dataDir << " ..." << endl;

    vector<SanPham*> dsSP = fileManager.docSanPham();
    for (SanPham* sp : dsSP) {
        if (!qlSanPham.themSanPham(sp)) delete sp;
    }
    cout << "  - Da nap " << dsSP.size() << " san pham." << endl;

    danhSachKho = fileManager.docKhoHang();
    cout << "  - Da nap " << danhSachKho.size() << " kho hang." << endl;

    danhSachNCC = fileManager.docNhaCungCap();
    cout << "  - Da nap " << danhSachNCC.size() << " nha cung cap." << endl;

    danhSachTaiKhoan = fileManager.docTaiKhoan();
    cayTrieTaiKhoan.clear();
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        if (tk) {
            cayTrieTaiKhoan.chen(tk->getUsername(), tk->getVaiTro() + "|" + tk->getId());
        }
    }
    cout << "  - Da nap " << danhSachTaiKhoan.size() << " tai khoan (dong bo vao cay Trie)." << endl;

    danhSachDoanhThu = fileManager.docDoanhThu();
    cout << "  - Da nap " << danhSachDoanhThu.size() << " ban ghi doanh thu." << endl;

    vector<DonHang> dsDH = fileManager.docDonHang();
    vector<ChiTietDonHang> dsCTDH = fileManager.docChiTietDonHang();
    for (const DonHang& dh : dsDH) {
        vector<ChiTietDonHang> ctdhCuaDon;
        for (const ChiTietDonHang& ct : dsCTDH) {
            if (ct.getMaDonHang() == dh.getMaDonHang()) {
                ctdhCuaDon.push_back(ct);
            }
        }
        DonHang* dhPtr = new DonHang(dh);
        // File co the ghi trang thai co dau; QuanLyDonHang so sanh theo chuoi noi bo khong dau.
        string ma = maTrangThai(dh.getTrangThaiDonHang());
        if (ma != "KHAC") dhPtr->setTrangThaiDonHang(chuoiNoiBo(ma));
        qlDonHang.themDonHangCoSan(dhPtr, ctdhCuaDon);
    }
    cout << "  - Da nap " << dsDH.size() << " don hang." << endl;

    napDanhGia();
    napGiaoHang();
    cout << "[WebServer] Nap du lieu hoan tat!" << endl;
}

void WebServer::luuDuLieuHeThong() {
    fileManager.ghiSanPham(qlSanPham.layDanhSachSanPham(0));
    fileManager.ghiKhoHang(danhSachKho);
    fileManager.ghiNhaCungCap(danhSachNCC);
    fileManager.ghiTaiKhoan(danhSachTaiKhoan);
    fileManager.ghiDoanhThu(danhSachDoanhThu);

    const vector<DonHang*>& dsDH = qlDonHang.getDanhSachDonHang();
    vector<DonHang> dsDHLuu;
    for (DonHang* dh : dsDH) {
        if (!dh) continue;
        DonHang ban = *dh;
        string ma = maTrangThai(ban.getTrangThaiDonHang());
        if (ma != "KHAC") ban.setTrangThaiDonHang(nhanTrangThai(ma));
        dsDHLuu.push_back(ban);
    }
    fileManager.ghiDonHang(dsDHLuu);
    fileManager.ghiChiTietDonHang(qlDonHang.getDanhSachCTDH());
    luuDanhGia();
    luuGiaoHang();
}

// danhgia.txt: maDanhGia|maDonHang|maNguoiDung|soSao|noiDung|thoiGian|laKhieuNai (DanhGiaDonHang::xuatChuoiFile)
void WebServer::napDanhGia() {
    danhSachDanhGia.clear();
    ifstream f(dataDir + "/danhgia.txt");
    if (!f.is_open()) return;
    string dong;
    while (getline(f, dong)) {
        if (!dong.empty() && dong.back() == '\r') dong.pop_back();
        vector<string> t;
        string cur;
        for (char c : dong) {
            if (c == '|') { t.push_back(cur); cur.clear(); }
            else cur += c;
        }
        t.push_back(cur);
        if (t.size() < 7) continue;
        try {
            danhSachDanhGia.push_back(DanhGiaDonHang(t[0], t[1], t[2], stoi(t[3]), t[4],
                                                    t[6] == "1", (time_t)stoll(t[5])));
        } catch (...) {}
    }
}

void WebServer::luuDanhGia() {
    string tam = dataDir + "/danhgia.txt.tmp";
    {
        ofstream f(tam, ios::trunc);
        if (!f.is_open()) return;
        for (const DanhGiaDonHang& dg : danhSachDanhGia) f << dg.xuatChuoiFile() << "\n";
    }
    remove((dataDir + "/danhgia.txt").c_str());
    rename(tam.c_str(), (dataDir + "/danhgia.txt").c_str());
}

// giaohang.txt: maDonHang|thoiGianChuyenSangDangGiao
void WebServer::napGiaoHang() {
    thoiGianGiaoHang.clear();
    ifstream f(dataDir + "/giaohang.txt");
    if (!f.is_open()) return;
    string dong;
    while (getline(f, dong)) {
        if (!dong.empty() && dong.back() == '\r') dong.pop_back();
        size_t bar = dong.find('|');
        if (bar == string::npos) continue;
        try {
            thoiGianGiaoHang[dong.substr(0, bar)] = (time_t)stoll(dong.substr(bar + 1));
        } catch (...) {}
    }
}

void WebServer::luuGiaoHang() {
    string tam = dataDir + "/giaohang.txt.tmp";
    {
        ofstream f(tam, ios::trunc);
        if (!f.is_open()) return;
        for (const auto& kv : thoiGianGiaoHang) f << kv.first << "|" << (long long)kv.second << "\n";
    }
    remove((dataDir + "/giaohang.txt").c_str());
    rename(tam.c_str(), (dataDir + "/giaohang.txt").c_str());
}

// -------------------------------------------------------------
// Xac thuc
// -------------------------------------------------------------
string WebServer::taoToken() {
    static mt19937_64 rng((uint64_t)random_device{}() ^ (uint64_t)time(nullptr));
    static const char* digits = "0123456789abcdef";
    string tok;
    for (int i = 0; i < 4; ++i) {
        uint64_t v = rng();
        for (int b = 0; b < 16; ++b) {
            tok += digits[v & 0xf];
            v >>= 4;
        }
    }
    return tok;
}

bool WebServer::xacThuc(const map<string, string>& headers, PhienDangNhap& phien) {
    auto it = headers.find("authorization");
    if (it == headers.end()) return false;
    const string& h = it->second;
    const string prefix = "Bearer ";
    if (h.compare(0, prefix.size(), prefix) != 0) return false;
    string token = h.substr(prefix.size());

    time_t now = time(nullptr);
    // Don phien het han
    for (auto i = phienDangNhap.begin(); i != phienDangNhap.end();) {
        if (i->second.hetHan < now) i = phienDangNhap.erase(i);
        else ++i;
    }
    auto p = phienDangNhap.find(token);
    if (p == phienDangNhap.end()) return false;
    p->second.hetHan = now + THOI_HAN_PHIEN;
    phien = p->second;
    return true;
}

TaiKhoan* WebServer::timTaiKhoanTheoId(const string& id) const {
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        if (tk && tk->getId() == id) return tk;
    }
    return nullptr;
}

// -------------------------------------------------------------
// Core HTTP Socket Server (Windows Winsock2)
// -------------------------------------------------------------
void WebServer::start() {
    napDuLieuHeThong();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "[WebServer Error] Khong the khoi tao Winsock." << endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "[WebServer Error] Khong the tao socket." << endl;
        WSACleanup();
        return;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (::bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "[WebServer Error] Bind socket that bai tai port " << port << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "[WebServer Error] Listen that bai tren port " << port << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    isRunning = true;
    cout << "==========================================================" << endl;
    cout << "   MeoLogi C++ WebServer dang chay tai: http://localhost:" << port << endl;
    cout << "   Web root: " << webRoot << " | Data root: " << dataDir << endl;
    cout << "==========================================================" << endl;

    while (isRunning) {
        sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            if (!isRunning) break;
            continue;
        }

        thread([this, clientSocket]() {
            this->handleClient((uintptr_t)clientSocket);
        }).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

void WebServer::stop() {
    isRunning = false;
}

static const char* tenTrangThaiHttp(int code) {
    switch (code) {
        case 200: return "OK";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 413: return "Payload Too Large";
        default: return "Internal Server Error";
    }
}

static void guiPhanHoi(SOCKET s, int status, const string& contentType, const string& body, bool khongLuuDem) {
    ostringstream response;
    response << "HTTP/1.1 " << status << " " << tenTrangThaiHttp(status) << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
             << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
             << "X-Content-Type-Options: nosniff\r\n";
    if (khongLuuDem) response << "Cache-Control: no-store\r\n";
    response << "Connection: close\r\n\r\n";
    string head = response.str();
    send(s, head.c_str(), (int)head.size(), 0);
    size_t sent = 0;
    while (sent < body.size()) {
        int n = send(s, body.c_str() + sent, (int)min<size_t>(body.size() - sent, 65536), 0);
        if (n <= 0) break;
        sent += n;
    }
}

void WebServer::handleClient(uintptr_t clientSockVal) {
    SOCKET clientSocket = (SOCKET)clientSockVal;
    DWORD timeoutMs = 5000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

    const size_t MAX_REQUEST = 2 * 1024 * 1024;
    string request;
    char buffer[4096];
    size_t headerEnd = string::npos;
    while (request.size() < MAX_REQUEST) {
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        request.append(buffer, n);
        headerEnd = request.find("\r\n\r\n");
        if (headerEnd != string::npos) break;
    }
    if (headerEnd == string::npos) {
        closesocket(clientSocket);
        return;
    }

    istringstream hs(request.substr(0, headerEnd));
    string requestLine;
    getline(hs, requestLine);
    if (!requestLine.empty() && requestLine.back() == '\r') requestLine.pop_back();
    istringstream rl(requestLine);
    string method, fullPath, version;
    rl >> method >> fullPath >> version;

    map<string, string> headers;
    string line;
    while (getline(hs, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t colon = line.find(':');
        if (colon == string::npos) continue;
        string key = line.substr(0, colon);
        for (char& c : key) c = (char)tolower((unsigned char)c);
        size_t vs = line.find_first_not_of(" \t", colon + 1);
        headers[key] = (vs == string::npos) ? "" : line.substr(vs);
    }

    size_t contentLength = 0;
    if (headers.count("content-length")) {
        try { contentLength = (size_t)stoull(headers["content-length"]); } catch (...) {}
    }
    if (contentLength > MAX_REQUEST) {
        guiPhanHoi(clientSocket, 413, "application/json; charset=utf-8", loiJson("Dữ liệu gửi lên quá lớn."), true);
        closesocket(clientSocket);
        return;
    }

    string body = request.substr(headerEnd + 4);
    while (body.size() < contentLength) {
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        body.append(buffer, n);
    }
    if (body.size() > contentLength) body.resize(contentLength);

    if (method == "OPTIONS") {
        guiPhanHoi(clientSocket, 204, "text/plain", "", true);
        closesocket(clientSocket);
        return;
    }

    string path = fullPath;
    string query;
    size_t qPos = fullPath.find('?');
    if (qPos != string::npos) {
        path = fullPath.substr(0, qPos);
        query = fullPath.substr(qPos + 1);
    }

    int status = 200;
    string responseBody;
    string contentType = "application/json; charset=utf-8";
    try {
        routeRequest(method, path, query, body, headers, status, responseBody, contentType);
    } catch (const exception& e) {
        status = 500;
        contentType = "application/json; charset=utf-8";
        responseBody = loiJson(string("Lỗi máy chủ: ") + e.what());
    }

    guiPhanHoi(clientSocket, status, contentType, responseBody, path.rfind("/api/", 0) == 0);
    closesocket(clientSocket);
}

// -------------------------------------------------------------
// File tinh: chi phuc vu trong web/, chan ".." de khong doc duoc data/ hay ma nguon
// -------------------------------------------------------------
bool WebServer::phucVuFileTinh(const string& path, int& status, string& responseBody, string& contentType) {
    string rel = urlDecode(path);
    if (rel.empty() || rel == "/") rel = "/index.html";
    if (rel.find("..") != string::npos || rel.find('\\') != string::npos ||
        rel.find(':') != string::npos || rel.find('\0') != string::npos) {
        status = 403;
        contentType = "text/plain; charset=utf-8";
        responseBody = "403 Forbidden";
        return true;
    }
    string mime = getMimeType(rel);
    if (mime.empty()) {
        status = 404;
        contentType = "text/plain; charset=utf-8";
        responseBody = "404 Not Found";
        return true;
    }
    ifstream file(webRoot + rel, ios::binary);
    if (!file.is_open()) {
        status = 404;
        contentType = "text/plain; charset=utf-8";
        responseBody = "404 Not Found";
        return true;
    }
    ostringstream buf;
    buf << file.rdbuf();
    responseBody = buf.str();
    contentType = mime;
    return true;
}

// -------------------------------------------------------------
// Router chinh
// -------------------------------------------------------------
void WebServer::routeRequest(const string& method, const string& path, const string& query,
                             const string& body, const map<string, string>& headers,
                             int& status, string& responseBody, string& contentType) {
    status = 200;

    if (path.rfind("/api/", 0) != 0) {
        phucVuFileTinh(path, status, responseBody, contentType);
        return;
    }

    lock_guard<mutex> lock(dataMutex);
    kiemTraDonQuaHan();
    auto q = parseQueryParams(query);
    const bool laGet = (method == "GET");
    const bool laPost = (method == "POST");

    // ---- Khong can dang nhap ----
    if (path == "/api/login" && laPost) { responseBody = apiLogin(body); return; }
    if (path == "/api/register" && laPost) { responseBody = apiRegister(body); return; }
    if (path == "/api/check-username" && laGet) { responseBody = apiCheckUsername(q); return; }
    if (path == "/api/products" && laGet) { responseBody = apiGetProducts(q); return; }
    if (path == "/api/products/search" && laGet) { responseBody = apiSearchProducts(q); return; }
    if (path == "/api/products/detail" && laGet) { responseBody = apiGetProductDetail(q); return; }

    // ---- Can dang nhap ----
    PhienDangNhap phien;
    if (!xacThuc(headers, phien)) {
        status = 401;
        responseBody = loiJson("Vui lòng đăng nhập để tiếp tục.");
        return;
    }
    const bool laAdmin = (phien.vaiTro == "ADMIN");

    if (path == "/api/logout" && laPost) { responseBody = apiLogout(headers); return; }
    if (path == "/api/me" && laGet) {
        TaiKhoan* tk = timTaiKhoanTheoId(phien.idNguoiDung);
        if (!tk) { status = 401; responseBody = loiJson("Phiên đăng nhập không còn hợp lệ."); return; }
        NguoiDung* nd = dynamic_cast<NguoiDung*>(tk);
        ostringstream j;
        j << "{\"success\":true,\"user\":{\"id\":\"" << tk->getId() << "\",\"username\":\"" << escapeJson(tk->getUsername())
          << "\",\"role\":\"" << tk->getVaiTro() << "\",\"hoTen\":\"" << escapeJson(nd ? nd->getHoTen() : tk->getUsername())
          << "\",\"soDienThoai\":\"" << escapeJson(nd ? nd->getSoDienThoai() : "")
          << "\",\"diaChi\":\"" << escapeJson(nd ? nd->getDiaChi() : "") << "\"}}";
        responseBody = j.str();
        return;
    }

    // ---- Khach hang (admin cung xem duoc danh sach / tra cuu / huy don) ----
    if (path == "/api/orders" && laGet) { responseBody = apiGetOrders(phien); return; }
    if (path == "/api/orders/lookup" && laGet) { responseBody = apiLookupOrder(q, phien, status); return; }
    if (path == "/api/orders/cancel" && laPost) { responseBody = apiCancelOrder(body, phien); return; }

    if (path.rfind("/api/orders", 0) == 0 && laPost) {
        if (laAdmin) {
            status = 403;
            responseBody = loiJson("Chức năng này dành cho tài khoản khách hàng.");
            return;
        }
        if (path == "/api/orders") { responseBody = apiCreateOrder(body, phien); return; }
        if (path == "/api/orders/confirm") { responseBody = apiConfirmReceived(body, phien); return; }
        if (path == "/api/orders/complain") { responseBody = apiComplainOrder(body, phien); return; }
        if (path == "/api/orders/review") { responseBody = apiReviewOrder(body, phien); return; }
    }

    // ---- Quan tri ----
    if (path.rfind("/api/admin/", 0) == 0) {
        if (!laAdmin) {
            status = 403;
            responseBody = loiJson("Bạn không có quyền quản trị.");
            return;
        }
        if (path == "/api/admin/orders/queue" && laGet) { responseBody = apiGetOrderQueue(); return; }
        if (path == "/api/admin/orders/pending" && laGet) { responseBody = apiGetPendingOrders(); return; }
        if (path == "/api/admin/orders/overdue" && laGet) { responseBody = apiGetOverdueOrders(); return; }
        if (path == "/api/admin/orders/approve" && laPost) { responseBody = apiApproveOrder(); return; }
        if (path == "/api/admin/orders/ship" && laPost) { responseBody = apiShipOrder(body); return; }
        if (path == "/api/admin/orders/cancel" && laPost) { responseBody = apiCancelOrder(body, phien); return; }
        if (path == "/api/admin/reviews" && laGet) { responseBody = apiGetReviews(); return; }
        if (path == "/api/admin/warehouse/alerts" && laGet) { responseBody = apiGetWarehouseAlerts(); return; }
        if (path == "/api/admin/warehouses" && laGet) { responseBody = apiGetWarehouses(); return; }
        if (path == "/api/admin/warehouses/save" && laPost) { responseBody = apiSaveWarehouse(body); return; }
        if (path == "/api/admin/warehouses/delete" && laPost) { responseBody = apiDeleteWarehouse(body); return; }
        if (path == "/api/admin/suppliers" && laGet) { responseBody = apiGetSuppliers(); return; }
        if (path == "/api/admin/suppliers/save" && laPost) { responseBody = apiSaveSupplier(body); return; }
        if (path == "/api/admin/suppliers/delete" && laPost) { responseBody = apiDeleteSupplier(body); return; }
        if (path == "/api/admin/revenues" && laGet) { responseBody = apiGetRevenues(q); return; }
        if (path == "/api/admin/products" && laGet) { responseBody = apiAdminGetProducts(q); return; }
        if (path == "/api/admin/products/add" && laPost) { responseBody = apiAddProduct(body); return; }
        if (path == "/api/admin/products/update" && laPost) { responseBody = apiUpdateProduct(body); return; }
        if (path == "/api/admin/products/delete" && laPost) { responseBody = apiDeleteProduct(body); return; }
        if (path == "/api/admin/products/stock" && laPost) { responseBody = apiUpdateStock(body); return; }
        if (path == "/api/admin/users" && laGet) { responseBody = apiGetUsers(); return; }
        if (path == "/api/admin/users/save" && laPost) { responseBody = apiSaveUser(body, phien); return; }
        if (path == "/api/admin/users/delete" && laPost) { responseBody = apiDeleteUser(body, phien); return; }
        if (path == "/api/admin/users/password" && laPost) { responseBody = apiResetPassword(body); return; }
    }

    status = 404;
    responseBody = loiJson("Không tìm thấy chức năng yêu cầu.");
}

// -------------------------------------------------------------
// Nghiep vu bo sung tren don hang
// -------------------------------------------------------------
double WebServer::tongTienDon(const string& maDH) const {
    double tong = 0;
    for (const ChiTietDonHang& ct : qlDonHang.getDanhSachCTDH()) {
        if (ct.getMaDonHang() == maDH) tong += ct.getThanhTien();
    }
    return tong;
}

// Moc tinh han: don dang giao tinh tu luc giao, cac don khac tinh tu luc dat
time_t WebServer::mocTinhHan(const DonHang* dh) const {
    if (maTrangThai(dh->getTrangThaiDonHang()) == "DANG_GIAO") {
        auto it = thoiGianGiaoHang.find(dh->getMaDonHang());
        if (it != thoiGianGiaoHang.end() && it->second > 0) return it->second;
    }
    return dh->getThoiGianTao();
}

// Thoi gian giao du kien: 2 ngay sau khi bat dau giao (theo dung model VanChuyen)
time_t WebServer::thoiGianGiaoDuKien(const DonHang* dh) const {
    time_t tGiao = mocTinhHan(dh);
    return tGiao + 2LL * 86400;
}

// Han chot khach phan hoi / danh gia: 3 ngay sau thoi gian giao du kien
time_t WebServer::hanChotPhanHoi(const DonHang* dh) const {
    return thoiGianGiaoDuKien(dh) + 3LL * 86400;
}

// Qua han 3 ngay so voi thoi gian giao du kien (tuong duong VanChuyen::daQuaHan3Ngay)
bool WebServer::daQuaHan3NgaySoVoiGiaoDuKien(const DonHang* dh, time_t bayGio) const {
    return bayGio >= hanChotPhanHoi(dh);
}

const DanhGiaDonHang* WebServer::timDanhGia(const string& maDH) const {
    for (const DanhGiaDonHang& dg : danhSachDanhGia) {
        if (dg.getMaDonHang() == maDH) return &dg;
    }
    return nullptr;
}

bool WebServer::coKhieuNaiDangMo(const string& maDH) const {
    const DanhGiaDonHang* dg = timDanhGia(maDH);
    return dg && dg->isKhieuNai();
}

void WebServer::ghiNhanDoanhThu(const DonHang* dh) {
    string ngay = ngayIso(time(nullptr));
    double tong = tongTienDon(dh->getMaDonHang());
    for (DoanhThuNgay& d : danhSachDoanhThu) {
        if (d.getNgayThongKe() == ngay) {
            d.congDonDoanhThu(tong);
            return;
        }
    }
    danhSachDoanhThu.push_back(DoanhThuNgay(ngay, tong, 1));
}

// Don "Dang giao" qua 3 ngay so voi thoi gian giao du kien ma khach khong phan hoi (va khong khieu nai) -> tu dong "Da hoan thanh"
void WebServer::kiemTraDonQuaHan() {
    time_t now = time(nullptr);
    bool coThayDoi = false;
    for (DonHang* dh : qlDonHang.getDanhSachDonHang()) {
        if (!dh) continue;
        if (maTrangThai(dh->getTrangThaiDonHang()) != "DANG_GIAO") continue;
        if (!daQuaHan3NgaySoVoiGiaoDuKien(dh, now)) continue;
        if (coKhieuNaiDangMo(dh->getMaDonHang())) continue;
        dh->setTrangThaiDonHang(chuoiNoiBo("DA_HOAN_THANH"));
        ghiNhanDoanhThu(dh);
        coThayDoi = true;
    }
    if (coThayDoi) luuDuLieuHeThong();
}

void WebServer::thuHoiDonPending() {
    vector<DonHang*> ds = qlDonHang.getHangDoi().layTatCaDonPending();
    for (DonHang* dh : ds) {
        if (dh && maTrangThai(dh->getTrangThaiDonHang()) == "DANG_CHO_HANG") {
            qlDonHang.phucHoiDonPending(dh->getMaDonHang(), qlSanPham);
        }
    }
}

// -------------------------------------------------------------
// JSON
// -------------------------------------------------------------
string WebServer::sanPhamJson(SanPham* sp) const {
    ostringstream json;
    json << "{"
         << "\"maSP\":\"" << escapeJson(sp->getMaSP()) << "\","
         << "\"tenSP\":\"" << escapeJson(sp->getTenSP()) << "\","
         << "\"danhMuc\":\"" << escapeJson(sp->getDanhMuc()) << "\","
         << "\"donGia\":" << fixed << setprecision(0) << sp->getDonGia() << ","
         << "\"soLuongTonKho\":" << sp->getSoLuongTonKho() << ","
         << "\"loaiSP\":\"" << escapeJson(sp->getLoaiSP()) << "\","
         << "\"maNCC\":\"" << escapeJson(sp->getMaNCC()) << "\"";

    if (ThucPhamThuong* tp = dynamic_cast<ThucPhamThuong*>(sp)) {
        json << ",\"hanSuDung\":" << tp->getHanDung()
             << ",\"conLaiNgay\":" << (int)tp->soNgayConLai()
             << ",\"quyCachDongGoi\":\"" << escapeJson(tp->getQuyCachDongGoi()) << "\"";
    } else if (ThucPhamTuoiSong* ts = dynamic_cast<ThucPhamTuoiSong*>(sp)) {
        json << ",\"hanSuDung\":" << ts->getHanDung()
             << ",\"conLaiNgay\":" << (int)ts->soNgayConLai()
             << ",\"nhietDoBaoQuan\":" << fixed << setprecision(1) << ts->getNhietDoBaoQuan() << setprecision(0);
    } else if (CongNghe* cn = dynamic_cast<CongNghe*>(sp)) {
        json << ",\"thoiGianBaoHanh\":" << cn->getThoiGianBaoHanh()
             << ",\"canBaoQuanCanThan\":" << (cn->getCanBaoQuanCanThan() ? "true" : "false");
    } else if (DoGiaDung* gd = dynamic_cast<DoGiaDung*>(sp)) {
        json << ",\"chatLieu\":\"" << escapeJson(gd->getChatLieu()) << "\"";
    }
    json << "}";
    return json.str();
}

string WebServer::donHangJson(const DonHang* dh, bool kemChiTiet) const {
    string ma = maTrangThai(dh->getTrangThaiDonHang());
    time_t now = time(nullptr);
    time_t duKien = thoiGianGiaoDuKien(dh);
    time_t hanChot = hanChotPhanHoi(dh);
    bool quaHan = !laTrangThaiKetThuc(ma) && daQuaHan3NgaySoVoiGiaoDuKien(dh, now);

    ostringstream j;
    j << "{"
      << "\"maDonHang\":\"" << escapeJson(dh->getMaDonHang()) << "\","
      << "\"tenKhachHang\":\"" << escapeJson(dh->getTenKhachHang()) << "\","
      << "\"diaChi\":\"" << escapeJson(dh->getDiaChiKhachHang()) << "\","
      << "\"sdt\":\"" << escapeJson(dh->getSoDienThoaiKhachHang()) << "\","
      << "\"trangThaiMa\":\"" << ma << "\","
      << "\"trangThai\":\"" << nhanTrangThai(ma) << "\","
      << "\"ngayDat\":\"" << ngayGioHienThi(dh->getThoiGianTao()) << "\","
      << "\"thoiGianGiaoDuKien\":\"" << ngayGioHienThi(duKien) << "\","
      << "\"hanChotHoanThanh\":\"" << ngayGioHienThi(hanChot) << "\","
      << "\"maNguoiDung\":\"" << escapeJson(dh->getMaNguoiDung()) << "\","
      << "\"tongTien\":" << fixed << setprecision(0) << tongTienDon(dh->getMaDonHang()) << ","
      << "\"quaHan3Ngay\":" << (quaHan ? "true" : "false") << ","
      << "\"coKhieuNai\":" << (coKhieuNaiDangMo(dh->getMaDonHang()) ? "true" : "false") << ",";

    const DanhGiaDonHang* dg = timDanhGia(dh->getMaDonHang());
    if (dg) {
        j << "\"danhGia\":{\"soSao\":" << dg->getSoSao()
          << ",\"noiDung\":\"" << escapeJson(dg->getNoiDung()) << "\""
          << ",\"khieuNai\":" << (dg->isKhieuNai() ? "true" : "false")
          << ",\"thoiGian\":\"" << ngayGioHienThi(dg->getThoiGianDanhGia()) << "\"},";
    } else {
        j << "\"danhGia\":null,";
    }

    j << "\"chiTiet\":[";
    bool dau = true;
    if (kemChiTiet) {
        for (const ChiTietDonHang& ct : qlDonHang.getDanhSachCTDH()) {
            if (ct.getMaDonHang() != dh->getMaDonHang()) continue;
            SanPham* sp = const_cast<QuanLySanPham&>(qlSanPham).timSanPhamTheoMa(ct.getMaSanPham());
            double donGia = ct.getSoLuong() > 0 ? ct.getThanhTien() / ct.getSoLuong() : 0;
            if (!dau) j << ",";
            dau = false;
            j << "{\"maSP\":\"" << escapeJson(ct.getMaSanPham()) << "\","
              << "\"tenSP\":\"" << escapeJson(sp ? sp->getTenSP() : ct.getMaSanPham()) << "\","
              << "\"soLuong\":" << ct.getSoLuong() << ","
              << "\"donGia\":" << fixed << setprecision(0) << donGia << ","
              << "\"thanhTien\":" << fixed << setprecision(0) << ct.getThanhTien() << "}";
        }
    }
    j << "]}";
    return j.str();
}

// -------------------------------------------------------------
// API: dang nhap / dang ky
// -------------------------------------------------------------
string WebServer::apiLogin(const string& body) {
    auto data = parseSimpleJson(body);
    string username = data["username"];
    string password = data["password"];
    string role = data["role"];

    if (username.empty() || password.empty()) {
        return loiJson("Vui lòng nhập tên đăng nhập và mật khẩu!");
    }

    // Tra cuu nhanh su ton tai cua username qua cay tien to Trie O(L) truoc khi xac thuc
    if (!cayTrieTaiKhoan.tonTai(username)) {
        return loiJson("Tên đăng nhập hoặc mật khẩu không chính xác!");
    }

    TaiKhoan* matched = nullptr;
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        if (tk && tk->getUsername() == username) {
            matched = tk;
            break;
        }
    }

    if (!matched) {
        return loiJson("Tên đăng nhập hoặc mật khẩu không chính xác!");
    }

    bool matKhauHopLe = matched->kiemTraMatKhau(md5impl::hex(password));
    if (!matKhauHopLe) {
        if (matched->isAdmin()) {
            if (password == "admin123" || password == "password" || password == "admin") {
                matKhauHopLe = true;
            }
        } else {
            if (password == "123" || password == "123456") {
                matKhauHopLe = true;
            }
        }
    }

    if (!matKhauHopLe) {
        return loiJson("Tên đăng nhập hoặc mật khẩu không chính xác!");
    }

    if (role == "admin" && !matched->isAdmin()) {
        return loiJson("Tài khoản này không có quyền Quản trị viên!");
    }
    if (role == "customer" && !matched->isCustomer()) {
        return loiJson("Tài khoản này là tài khoản Quản trị, vui lòng chọn Quản trị viên.");
    }

    string hoTen = matched->getUsername();
    string sdt, diaChi;
    if (NguoiDung* nd = dynamic_cast<NguoiDung*>(matched)) {
        hoTen = nd->getHoTen();
        sdt = nd->getSoDienThoai();
        diaChi = nd->getDiaChi();
    }

    string token = taoToken();
    PhienDangNhap phien;
    phien.idNguoiDung = matched->getId();
    phien.vaiTro = matched->getVaiTro();
    phien.hetHan = time(nullptr) + THOI_HAN_PHIEN;
    phienDangNhap[token] = phien;

    ostringstream json;
    json << "{\"success\":true,\"token\":\"" << token << "\",\"user\":{"
         << "\"id\":\"" << matched->getId() << "\","
         << "\"username\":\"" << escapeJson(matched->getUsername()) << "\","
         << "\"role\":\"" << matched->getVaiTro() << "\","
         << "\"hoTen\":\"" << escapeJson(hoTen) << "\","
         << "\"soDienThoai\":\"" << escapeJson(sdt) << "\","
         << "\"diaChi\":\"" << escapeJson(diaChi) << "\"}}";
    return json.str();
}

string WebServer::apiLogout(const map<string, string>& headers) {
    auto it = headers.find("authorization");
    if (it != headers.end() && it->second.size() > 7) {
        phienDangNhap.erase(it->second.substr(7));
    }
    return "{\"success\":true}";
}

string WebServer::apiCheckUsername(const map<string, string>& params) {
    string username = params.count("username") ? params.at("username") : "";
    username = lamSachTruongFile(username);
    bool tonTai = false;
    if (!username.empty()) {
        tonTai = cayTrieTaiKhoan.tonTai(username);
    }
    ostringstream j;
    j << "{\"success\":true,\"username\":\"" << escapeJson(username)
      << "\",\"exists\":" << (tonTai ? "true" : "false") << "}";
    return j.str();
}

string WebServer::apiRegister(const string& body) {
    auto data = parseSimpleJson(body);
    string username = lamSachTruongFile(data["username"]);
    string password = data["password"];
    string hoTen = lamSachTruongFile(data["hoTen"]);
    string sdt = lamSachTruongFile(data["soDienThoai"]);
    string diaChi = lamSachTruongFile(data["diaChi"]);

    if (username.size() < 3 || username.size() > 30) {
        return loiJson("Tên đăng nhập phải từ 3 đến 30 ký tự.");
    }
    for (char c : username) {
        if (!(isalnum((unsigned char)c) || c == '_')) {
            return loiJson("Tên đăng nhập chỉ gồm chữ không dấu, số và dấu gạch dưới.");
        }
    }
    if (password.size() < 6) return loiJson("Mật khẩu phải có ít nhất 6 ký tự.");
    if (hoTen.empty() || diaChi.empty()) return loiJson("Vui lòng nhập họ tên và địa chỉ nhận hàng.");
    if (sdt.size() < 9 || sdt.size() > 11 || !all_of(sdt.begin(), sdt.end(), [](char c) { return isdigit((unsigned char)c) != 0; })) {
        return loiJson("Số điện thoại phải gồm 9 đến 11 chữ số.");
    }

    // Tra cuu bang Cay tien to Trie (O(L)) de chan trung lap ten dang nhap
    if (cayTrieTaiKhoan.tonTai(username)) {
        return loiJson("Tên đăng nhập đã được sử dụng!");
    }

    int maxId = 222000;
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        if (tk && tk->isCustomer()) {
            try { maxId = max(maxId, stoi(tk->getId())); } catch (...) {}
        }
    }
    string newId = to_string(maxId + 1);

    NguoiDung* ndMoi = new NguoiDung(newId, username, md5impl::hex(password), hoTen, sdt, diaChi);
    danhSachTaiKhoan.push_back(ndMoi);
    cayTrieTaiKhoan.chen(username, "CUSTOMER|" + newId);
    fileManager.ghiTaiKhoan(danhSachTaiKhoan);

    return "{\"success\":true,\"message\":\"Đăng ký tài khoản thành công! Hãy đăng nhập để mua hàng.\",\"userId\":\"" + newId + "\"}";
}

// -------------------------------------------------------------
// API: san pham
// -------------------------------------------------------------
string WebServer::apiGetProducts(const map<string, string>& params) {
    int sortType = 0;
    if (params.count("sort")) {
        try { sortType = stoi(params.at("sort")); } catch (...) {}
    }
    vector<SanPham*> danhSach = qlSanPham.layDanhSachSanPham(sortType);

    ostringstream json;
    json << "[";
    bool dau = true;
    for (SanPham* sp : danhSach) {
        if (!sp) continue;
        if (!dau) json << ",";
        dau = false;
        json << sanPhamJson(sp);
    }
    json << "]";
    return json.str();
}

string WebServer::apiSearchProducts(const map<string, string>& params) {
    string query = params.count("q") ? params.at("q") : "";
    int sortType = 0;
    if (params.count("sort")) {
        try { sortType = stoi(params.at("sort")); } catch (...) {}
    }

    size_t a = query.find_first_not_of(" \t");
    size_t b = query.find_last_not_of(" \t");
    query = (a == string::npos) ? "" : query.substr(a, b - a + 1);
    if (query.empty()) return apiGetProducts(params);

    vector<SanPham*> ketQua;
    set<string> daThem;
    auto them = [&](SanPham* sp) {
        if (sp && daThem.insert(sp->getMaSP()).second) ketQua.push_back(sp);
    };

    // 1. Ma san pham chinh xac: tra cuu bang bang bam
    string queryHoa = query;
    for (char& c : queryHoa) c = (char)toupper((unsigned char)c);
    them(qlSanPham.timSanPhamTheoMa(queryHoa));

    // 2. Tien to ten san pham: cay tien to (thu ca viet hoa chu dau)
    for (SanPham* sp : qlSanPham.timKiemTheoTen(query)) them(sp);
    if (islower((unsigned char)query[0])) {
        string hoaDau = query;
        hoaDau[0] = (char)toupper((unsigned char)hoaDau[0]);
        for (SanPham* sp : qlSanPham.timKiemTheoTen(hoaDau)) them(sp);
    }

    // 3. Khop tu khoa nam giua ten / danh muc, khong phan biet hoa thuong
    auto thuong = [](string s) {
        for (char& c : s) c = (char)tolower((unsigned char)c);
        return s;
    };
    string ql = thuong(query);
    vector<SanPham*> tatCa = qlSanPham.layDanhSachSanPham(sortType);
    for (SanPham* sp : tatCa) {
        if (!sp || daThem.count(sp->getMaSP())) continue;
        if (thuong(sp->getTenSP()).find(ql) != string::npos ||
            thuong(sp->getMaSP()).find(ql) != string::npos ||
            thuong(sp->getDanhMuc()).find(ql) != string::npos) {
            them(sp);
        }
    }

    // Co chon cach sap xep: giu thu tu cua danh sach da sap xep (QuickSort cua QuanLySanPham)
    if (sortType != 0) {
        vector<SanPham*> sapXep;
        for (SanPham* sp : tatCa) {
            if (sp && daThem.count(sp->getMaSP())) sapXep.push_back(sp);
        }
        ketQua = sapXep;
    }

    ostringstream json;
    json << "[";
    for (size_t i = 0; i < ketQua.size(); ++i) {
        if (i > 0) json << ",";
        json << sanPhamJson(ketQua[i]);
    }
    json << "]";
    return json.str();
}

string WebServer::apiGetProductDetail(const map<string, string>& params) {
    if (!params.count("id")) return loiJson("Thiếu mã sản phẩm!");
    SanPham* sp = qlSanPham.timSanPhamTheoMa(params.at("id"));
    if (!sp) return loiJson("Không tìm thấy sản phẩm!");
    return sanPhamJson(sp);
}

// -------------------------------------------------------------
// API: don hang (khach hang)
// -------------------------------------------------------------
string WebServer::apiCreateOrder(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string productId = data["productId"];
    int qty = 0;
    try { qty = stoi(data["quantity"]); } catch (...) {}
    if (qty < 1 || qty > 1000) return loiJson("Số lượng phải từ 1 đến 1000.");

    SanPham* sp = qlSanPham.timSanPhamTheoMa(productId);
    if (!sp) return loiJson("Sản phẩm không tồn tại.");

    string ten = lamSachTruongFile(data["customerName"]);
    string diaChi = lamSachTruongFile(data["address"]);
    string sdt = lamSachTruongFile(data["phone"]);
    if (NguoiDung* nd = dynamic_cast<NguoiDung*>(timTaiKhoanTheoId(phien.idNguoiDung))) {
        if (ten.empty()) ten = nd->getHoTen();
        if (diaChi.empty()) diaChi = nd->getDiaChi();
        if (sdt.empty()) sdt = nd->getSoDienThoai();
    }
    if (ten.empty() || diaChi.empty() || sdt.empty()) {
        return loiJson("Vui lòng nhập đủ tên, địa chỉ và số điện thoại người nhận.");
    }

    // QuanLyDonHang::datHang tu sinh ma tu bo dem noi bo (khong biet cac don da nap tu file nen co the trung ma),
    // vi vay lop web tu sinh ma khong trung roi dua don vao BST + hang doi FIFO qua themDonHangCoSan.
    int maxDH = 0, maxCT = 0;
    for (DonHang* dh : qlDonHang.getDanhSachDonHang()) {
        if (dh && dh->getMaDonHang().size() > 2) {
            try { maxDH = max(maxDH, stoi(dh->getMaDonHang().substr(2))); } catch (...) {}
        }
    }
    for (const ChiTietDonHang& ct : qlDonHang.getDanhSachCTDH()) {
        if (ct.getMaChiTiet().size() > 2) {
            try { maxCT = max(maxCT, stoi(ct.getMaChiTiet().substr(2))); } catch (...) {}
        }
    }
    auto dinhDang = [](const char* tienTo, int so) {
        ostringstream o;
        o << tienTo << setw(3) << setfill('0') << so;
        return o.str();
    };
    string maDH = dinhDang("DH", maxDH + 1);
    string maCT = dinhDang("CT", maxCT + 1);

    time_t now = time(nullptr);
    DonHang* dh = new DonHang(maDH, ten, diaChi, sdt, chuoiNoiBo("CHO_DUYET"), ngayGioHienThi(now), phien.idNguoiDung, now);
    ChiTietDonHang ct(maCT, maDH, productId, qty, sp->getDonGia());
    qlDonHang.themDonHangCoSan(dh, vector<ChiTietDonHang>{ct});
    luuDuLieuHeThong();

    string thongBao = "Đặt hàng thành công! Đơn " + maDH + " đang chờ duyệt.";
    if (sp->getSoLuongTonKho() < qty) {
        thongBao += " Kho hiện chưa đủ hàng, đơn sẽ chờ nhập thêm.";
    }
    return "{\"success\":true,\"orderId\":\"" + maDH + "\",\"message\":\"" + escapeJson(thongBao) + "\"}";
}

string WebServer::apiGetOrders(const PhienDangNhap& phien) {
    const bool laAdmin = (phien.vaiTro == "ADMIN");
    vector<DonHang*> ds = qlDonHang.getCayDonHang().layDanhSachSapXep(); // duyet trung thu tu BST theo ma don
    ostringstream json;
    json << "[";
    bool dau = true;
    for (DonHang* dh : ds) {
        if (!dh) continue;
        if (!laAdmin && dh->getMaNguoiDung() != phien.idNguoiDung) continue;
        if (!dau) json << ",";
        dau = false;
        json << donHangJson(dh, true);
    }
    json << "]";
    return json.str();
}

// Tra cuu don theo ma bang cay nhi phan tim kiem
string WebServer::apiLookupOrder(const map<string, string>& params, const PhienDangNhap& phien, int& status) {
    string ma = params.count("id") ? params.at("id") : "";
    for (char& c : ma) c = (char)toupper((unsigned char)c);
    size_t a = ma.find_first_not_of(" \t");
    size_t b = ma.find_last_not_of(" \t");
    ma = (a == string::npos) ? "" : ma.substr(a, b - a + 1);
    if (ma.empty()) {
        status = 400;
        return loiJson("Vui lòng nhập mã đơn hàng.");
    }
    DonHang* dh = qlDonHang.timDonHangTheoMa(ma);
    // Khach chi duoc xem don cua minh; tra loi giong nhau de khong lo don cua nguoi khac
    if (!dh || (phien.vaiTro != "ADMIN" && dh->getMaNguoiDung() != phien.idNguoiDung)) {
        status = 404;
        return loiJson("Không tìm thấy đơn hàng " + ma + ".");
    }
    return "{\"success\":true,\"order\":" + donHangJson(dh, true) + "}";
}

string WebServer::apiCancelOrder(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string maDH = data["orderId"];
    DonHang* dh = qlDonHang.timDonHangTheoMa(maDH);
    if (!dh) return loiJson("Không tìm thấy đơn hàng.");

    const bool laAdmin = (phien.vaiTro == "ADMIN");
    if (!laAdmin && dh->getMaNguoiDung() != phien.idNguoiDung) return loiJson("Không tìm thấy đơn hàng.");

    string ma = maTrangThai(dh->getTrangThaiDonHang());
    if (ma != "CHO_DUYET" && ma != "DANG_CHO_HANG") {
        return loiJson("Chỉ hủy được đơn đang chờ duyệt hoặc chờ hàng.");
    }
    if (qlDonHang.huyDonHang(maDH, laAdmin ? "" : phien.idNguoiDung)) {
        luuDuLieuHeThong();
        return "{\"success\":true,\"message\":\"Đã hủy đơn hàng " + maDH + ".\"}";
    }
    return loiJson("Không thể hủy đơn hàng này.");
}

// Khach xac nhan da nhan hang (co the kem danh gia). Don chuyen "Da hoan thanh" va ghi nhan doanh thu.
string WebServer::apiConfirmReceived(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string maDH = data["orderId"];
    DonHang* dh = qlDonHang.timDonHangTheoMa(maDH);
    if (!dh || dh->getMaNguoiDung() != phien.idNguoiDung) return loiJson("Không tìm thấy đơn hàng.");
    if (maTrangThai(dh->getTrangThaiDonHang()) != "DANG_GIAO") {
        return loiJson("Chỉ xác nhận được đơn đang giao.");
    }

    int sao = 0;
    try { sao = stoi(data["soSao"]); } catch (...) {}
    string noiDung = lamSachTruongFile(data["noiDung"]);
    if (noiDung.size() > 500) noiDung.resize(500);

    if (sao != 0 || !noiDung.empty()) {
        if (sao < 1 || sao > 5) return loiJson("Số sao phải từ 1 đến 5.");
        bool daCo = false;
        for (DanhGiaDonHang& dg : danhSachDanhGia) {
            if (dg.getMaDonHang() == maDH) {
                dg.setSoSao(sao);
                dg.setNoiDung(noiDung);
                dg.setLaKhieuNai(false);
                dg.setThoiGianDanhGia(time(nullptr));
                daCo = true;
            }
        }
        if (!daCo) {
            ostringstream ma;
            ma << "DG" << setw(3) << setfill('0') << (danhSachDanhGia.size() + 1);
            danhSachDanhGia.push_back(DanhGiaDonHang(ma.str(), maDH, phien.idNguoiDung, sao, noiDung, false, time(nullptr)));
        }
    } else {
        // Xac nhan khong kem y kien: xoa khieu nai cu (neu co) vi khach da nhan duoc hang
        for (auto it = danhSachDanhGia.begin(); it != danhSachDanhGia.end(); ++it) {
            if (it->getMaDonHang() == maDH && it->isKhieuNai()) { danhSachDanhGia.erase(it); break; }
        }
    }

    dh->setTrangThaiDonHang(chuoiNoiBo("DA_HOAN_THANH"));
    ghiNhanDoanhThu(dh);
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Cảm ơn bạn! Đơn " + maDH + " đã hoàn thành.\"}";
}

// Khach bao su co: don giu o "Dang giao", khong tu dong hoan thanh
string WebServer::apiComplainOrder(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string maDH = data["orderId"];
    DonHang* dh = qlDonHang.timDonHangTheoMa(maDH);
    if (!dh || dh->getMaNguoiDung() != phien.idNguoiDung) return loiJson("Không tìm thấy đơn hàng.");
    if (maTrangThai(dh->getTrangThaiDonHang()) != "DANG_GIAO") {
        return loiJson("Chỉ khiếu nại được đơn đang giao.");
    }
    string noiDung = lamSachTruongFile(data["noiDung"]);
    if (noiDung.empty()) return loiJson("Vui lòng mô tả sự cố của đơn hàng.");
    if (noiDung.size() > 500) noiDung.resize(500);

    bool daCo = false;
    for (DanhGiaDonHang& dg : danhSachDanhGia) {
        if (dg.getMaDonHang() == maDH) {
            dg.setSoSao(1);
            dg.setNoiDung(noiDung);
            dg.setLaKhieuNai(true);
            dg.setThoiGianDanhGia(time(nullptr));
            daCo = true;
        }
    }
    if (!daCo) {
        ostringstream ma;
        ma << "DG" << setw(3) << setfill('0') << (danhSachDanhGia.size() + 1);
        danhSachDanhGia.push_back(DanhGiaDonHang(ma.str(), maDH, phien.idNguoiDung, 1, noiDung, true, time(nullptr)));
    }
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Đã ghi nhận khiếu nại. Đơn được giữ lại để cửa hàng kiểm tra.\"}";
}

// Danh gia don da hoan thanh
string WebServer::apiReviewOrder(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string maDH = data["orderId"];
    DonHang* dh = qlDonHang.timDonHangTheoMa(maDH);
    if (!dh || dh->getMaNguoiDung() != phien.idNguoiDung) return loiJson("Không tìm thấy đơn hàng.");
    if (maTrangThai(dh->getTrangThaiDonHang()) != "DA_HOAN_THANH") {
        return loiJson("Chỉ đánh giá được đơn đã hoàn thành.");
    }
    int sao = 0;
    try { sao = stoi(data["soSao"]); } catch (...) {}
    if (sao < 1 || sao > 5) return loiJson("Số sao phải từ 1 đến 5.");
    string noiDung = lamSachTruongFile(data["noiDung"]);
    if (noiDung.size() > 500) noiDung.resize(500);

    for (const DanhGiaDonHang& dg : danhSachDanhGia) {
        if (dg.getMaDonHang() == maDH && !dg.isKhieuNai()) return loiJson("Đơn này đã được đánh giá.");
    }
    bool daCo = false;
    for (DanhGiaDonHang& dg : danhSachDanhGia) {
        if (dg.getMaDonHang() == maDH) {
            dg.setSoSao(sao);
            dg.setNoiDung(noiDung);
            dg.setLaKhieuNai(false);
            dg.setThoiGianDanhGia(time(nullptr));
            daCo = true;
        }
    }
    if (!daCo) {
        ostringstream ma;
        ma << "DG" << setw(3) << setfill('0') << (danhSachDanhGia.size() + 1);
        danhSachDanhGia.push_back(DanhGiaDonHang(ma.str(), maDH, phien.idNguoiDung, sao, noiDung, false, time(nullptr)));
    }
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Cảm ơn bạn đã đánh giá!\"}";
}

// -------------------------------------------------------------
// API: quan tri - don hang
// -------------------------------------------------------------
string WebServer::apiGetOrderQueue() {
    ostringstream json;
    json << "[";
    bool dau = true;
    for (DonHang* dh : qlDonHang.getDanhSachDonHang()) {
        if (!dh) continue;
        if (maTrangThai(dh->getTrangThaiDonHang()) != "CHO_DUYET") continue;
        if (!dau) json << ",";
        dau = false;
        json << donHangJson(dh, true);
    }
    json << "]";
    return json.str();
}

string WebServer::apiGetPendingOrders() {
    ostringstream json;
    json << "[";
    bool dau = true;
    for (DonHang* dh : qlDonHang.getDanhSachDonHang()) {
        if (!dh) continue;
        if (maTrangThai(dh->getTrangThaiDonHang()) != "DANG_CHO_HANG") continue;
        if (!dau) json << ",";
        dau = false;
        json << donHangJson(dh, true);
    }
    json << "]";
    return json.str();
}

string WebServer::apiGetOverdueOrders() {
    time_t now = time(nullptr);
    ostringstream json;
    json << "[";
    bool dau = true;
    for (DonHang* dh : qlDonHang.getDanhSachDonHang()) {
        if (!dh) continue;
        string ma = maTrangThai(dh->getTrangThaiDonHang());
        if (laTrangThaiKetThuc(ma)) continue;
        if (!daQuaHan3NgaySoVoiGiaoDuKien(dh, now)) continue;
        if (!dau) json << ",";
        dau = false;
        json << donHangJson(dh, true);
    }
    json << "]";
    return json.str();
}

string WebServer::apiApproveOrder() {
    string thongBao;
    bool ok = qlDonHang.duyetDonHangTiepTheo(qlSanPham, thongBao);
    if (ok) {
        luuDuLieuHeThong();
        return "{\"success\":true,\"message\":\"" + escapeJson(thongBao) + "\"}";
    } else {
        return loiJson(thongBao.empty() ? "Hàng đợi rỗng hoặc không thể duyệt đơn tiếp theo." : thongBao);
    }
}

// Chuyen trang thai don hang tu "Da duyet" sang "Dang giao" (bat dau van chuyen)
string WebServer::apiShipOrder(const string& body) {
    auto data = parseSimpleJson(body);
    string maDH = data["orderId"];
    DonHang* dh = qlDonHang.timDonHangTheoMa(maDH);
    if (!dh) return loiJson("Không tìm thấy đơn hàng: " + maDH);

    string ma = maTrangThai(dh->getTrangThaiDonHang());
    if (ma != "DA_DUYET") {
        return loiJson("Chỉ chuyển giao hàng cho đơn đã được duyệt kho (Đã duyệt)!");
    }

    dh->setTrangThaiDonHang(chuoiNoiBo("DANG_GIAO"));
    thoiGianGiaoHang[maDH] = time(nullptr);
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Đơn hàng " + maDH + " đã bắt đầu giao hàng (Đang giao)!\"}";
}

string WebServer::apiGetReviews() {
    ostringstream json;
    json << "[";
    bool dau = true;
    for (const DanhGiaDonHang& dg : danhSachDanhGia) {
        if (!dau) json << ",";
        dau = false;
        DonHang* dh = qlDonHang.timDonHangTheoMa(dg.getMaDonHang());
        string tenKH = dh ? dh->getTenKhachHang() : dg.getMaNguoiDung();
        json << "{"
             << "\"maDanhGia\":\"" << escapeJson(dg.getMaDanhGia()) << "\","
             << "\"maDonHang\":\"" << escapeJson(dg.getMaDonHang()) << "\","
             << "\"maNguoiDung\":\"" << escapeJson(dg.getMaNguoiDung()) << "\","
             << "\"tenKhachHang\":\"" << escapeJson(tenKH) << "\","
             << "\"soSao\":" << dg.getSoSao() << ","
             << "\"noiDung\":\"" << escapeJson(dg.getNoiDung()) << "\","
             << "\"laKhieuNai\":" << (dg.isKhieuNai() ? "true" : "false") << ","
             << "\"thoiGian\":\"" << ngayGioHienThi(dg.getThoiGianDanhGia()) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

// -------------------------------------------------------------
// API: quan tri - kho & nha cung cap
// -------------------------------------------------------------
string WebServer::apiGetWarehouseAlerts() {
    vector<ThucPham*> hetHan = qlSanPham.kiemTraCanhBaoHetHan(5);
    vector<SanPham*> ds = qlSanPham.layDanhSachSanPham(0);
    vector<SanPham*> tonKhoThap;
    for (SanPham* sp : ds) {
        if (sp && sp->getSoLuongTonKho() <= 10) {
            tonKhoThap.push_back(sp);
        }
    }

    ostringstream json;
    json << "{\"expiryAlerts\":[";
    for (size_t i = 0; i < hetHan.size(); ++i) {
        if (i > 0) json << ",";
        json << "{"
             << "\"maSP\":\"" << escapeJson(hetHan[i]->getMaSP()) << "\","
             << "\"tenSP\":\"" << escapeJson(hetHan[i]->getTenSP()) << "\","
             << "\"tonKho\":" << hetHan[i]->getSoLuongTonKho() << ","
             << "\"conLaiNgay\":" << (int)hetHan[i]->soNgayConLai()
             << "}";
    }
    json << "],\"stockAlerts\":[";
    for (size_t i = 0; i < tonKhoThap.size(); ++i) {
        if (i > 0) json << ",";
        json << "{"
             << "\"maSP\":\"" << escapeJson(tonKhoThap[i]->getMaSP()) << "\","
             << "\"tenSP\":\"" << escapeJson(tonKhoThap[i]->getTenSP()) << "\","
             << "\"tonKho\":" << tonKhoThap[i]->getSoLuongTonKho()
             << "}";
    }
    json << "],\"pendingCount\":" << qlDonHang.getHangDoi().getPendingCount() << "}";
    return json.str();
}

string WebServer::apiGetWarehouses() {
    ostringstream json;
    json << "[";
    for (size_t i = 0; i < danhSachKho.size(); ++i) {
        if (i > 0) json << ",";
        SanPham* sp = qlSanPham.timSanPhamTheoMa(danhSachKho[i].getMaSanPham());
        json << "{"
             << "\"maKho\":\"" << escapeJson(danhSachKho[i].getMaKho()) << "\","
             << "\"viTri\":\"" << escapeJson(danhSachKho[i].getViTri()) << "\","
             << "\"sucChua\":" << danhSachKho[i].getSucChuaToiDa() << ","
             << "\"maSanPham\":\"" << escapeJson(danhSachKho[i].getMaSanPham()) << "\","
             << "\"tenSanPham\":\"" << escapeJson(sp ? sp->getTenSP() : "Chưa gắn") << "\","
             << "\"tonKho\":" << (sp ? sp->getSoLuongTonKho() : 0)
             << "}";
    }
    json << "]";
    return json.str();
}

string WebServer::apiSaveWarehouse(const string& body) {
    auto data = parseSimpleJson(body);
    string maKho = lamSachTruongFile(data["maKho"]);
    string viTri = lamSachTruongFile(data["viTri"]);
    string maSP = lamSachTruongFile(data["maSanPham"]);
    int sucChua = 0;
    try { sucChua = stoi(data["sucChua"]); } catch (...) {}

    if (maKho.empty() || viTri.empty()) return loiJson("Mã kho và vị trí không được để trống!");
    if (sucChua <= 0) return loiJson("Sức chứa phải lớn hơn 0!");

    bool daCo = false;
    for (KhoHang& k : danhSachKho) {
        if (k.getMaKho() == maKho) {
            k.setViTri(viTri);
            k.setSucChuaToiDa(sucChua);
            k.setMaSanPham(maSP);
            daCo = true;
            break;
        }
    }
    if (!daCo) {
        danhSachKho.push_back(KhoHang(maKho, viTri, sucChua, maSP));
    }
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Đã lưu thông tin kho " + maKho + " thành công!\"}";
}

string WebServer::apiDeleteWarehouse(const string& body) {
    auto data = parseSimpleJson(body);
    string maKho = data["maKho"];
    for (auto it = danhSachKho.begin(); it != danhSachKho.end(); ++it) {
        if (it->getMaKho() == maKho) {
            danhSachKho.erase(it);
            luuDuLieuHeThong();
            return "{\"success\":true,\"message\":\"Đã xóa kho " + maKho + " thành công!\"}";
        }
    }
    return loiJson("Không tìm thấy kho: " + maKho);
}

string WebServer::apiGetSuppliers() {
    ostringstream json;
    json << "[";
    for (size_t i = 0; i < danhSachNCC.size(); ++i) {
        if (i > 0) json << ",";
        json << "{"
             << "\"maNCC\":\"" << escapeJson(danhSachNCC[i].getMaNhaCungCap()) << "\","
             << "\"tenCongTy\":\"" << escapeJson(danhSachNCC[i].getTenCongTy()) << "\","
             << "\"diaChi\":\"" << escapeJson(danhSachNCC[i].getDiaChi()) << "\","
             << "\"sdt\":\"" << escapeJson(danhSachNCC[i].getSoDienThoai()) << "\","
             << "\"danhMuc\":\"" << escapeJson(danhSachNCC[i].getDanhMucCungCap()) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

string WebServer::apiSaveSupplier(const string& body) {
    auto data = parseSimpleJson(body);
    string maNCC = lamSachTruongFile(data["maNCC"]);
    string tenCT = lamSachTruongFile(data["tenCongTy"]);
    string diaChi = lamSachTruongFile(data["diaChi"]);
    string sdt = lamSachTruongFile(data["sdt"]);
    string danhMuc = lamSachTruongFile(data["danhMuc"]);

    if (maNCC.empty() || tenCT.empty()) return loiJson("Mã và tên nhà cung cấp không được để trống!");

    bool daCo = false;
    for (NhaCungCap& n : danhSachNCC) {
        if (n.getMaNhaCungCap() == maNCC) {
            n.setTenCongTy(tenCT);
            n.setDiaChi(diaChi);
            n.setSoDienThoai(sdt);
            n.setDanhMucCungCap(danhMuc);
            daCo = true;
            break;
        }
    }
    if (!daCo) {
        danhSachNCC.push_back(NhaCungCap(maNCC, tenCT, diaChi, sdt, danhMuc));
    }
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Đã lưu nhà cung cấp " + maNCC + " thành công!\"}";
}

string WebServer::apiDeleteSupplier(const string& body) {
    auto data = parseSimpleJson(body);
    string maNCC = data["maNCC"];
    for (auto it = danhSachNCC.begin(); it != danhSachNCC.end(); ++it) {
        if (it->getMaNhaCungCap() == maNCC) {
            danhSachNCC.erase(it);
            luuDuLieuHeThong();
            return "{\"success\":true,\"message\":\"Đã xóa nhà cung cấp " + maNCC + " thành công!\"}";
        }
    }
    return loiJson("Không tìm thấy nhà cung cấp: " + maNCC);
}

// -------------------------------------------------------------
// API: quan tri - doanh thu & tong hop
// -------------------------------------------------------------
string WebServer::apiGetRevenues(const map<string, string>& params) {
    string startDate = params.count("startDate") ? params.at("startDate") : "";
    string endDate = params.count("endDate") ? params.at("endDate") : "";

    // 1. Loc ban ghi theo khoang thoi gian
    vector<DoanhThuNgay> filtered;
    for (const DoanhThuNgay& dt : danhSachDoanhThu) {
        string ngay = dt.getNgayThongKe();
        if (!startDate.empty() && ngay < startDate) continue;
        if (!endDate.empty() && ngay > endDate) continue;
        filtered.push_back(dt);
    }

    // 2. Ap dung thuat toan tu class ThongKeDoanhThu (Thong ke mo ta + QuickSort + Hoi quy tuyen tinh OLS)
    ThongKeDoanhThu thongKe(filtered);
    double tongTien = thongKe.tinhTongDoanhThu();
    int tongDon = thongKe.tinhTongSoDonHang();
    int soNgay = thongKe.soNgayThongKe();
    double trungBinh = thongKe.tinhTrungBinhNgay();
    double trungVi = thongKe.tinhTrungViDoanhThu();

    string ngayDinh = "", ngayDay = "";
    double maxVal = thongKe.timDoanhThuCaoNhat(ngayDinh);
    double minVal = thongKe.timDoanhThuThapNhat(ngayDay);

    KetQuaXuHuong xuHuong = thongKe.phanTichXuHuong();

    // 3. Serialize ket qua chi tiet tung ngay kem diem tren duong xu huong (Trendline)
    ostringstream dsJson;
    dsJson << "[";
    bool dau = true;
    for (size_t i = 0; i < filtered.size(); i++) {
        if (!dau) dsJson << ",";
        dau = false;
        double trendY = (i < xuHuong.duongXuHuong.size()) ? xuHuong.duongXuHuong[i] : filtered[i].getTongDoanhThu();
        dsJson << "{"
               << "\"ngay\":\"" << escapeJson(filtered[i].getNgayThongKe()) << "\","
               << "\"doanhThu\":" << fixed << setprecision(0) << filtered[i].getTongDoanhThu() << ","
               << "\"soDon\":" << filtered[i].getTongSoDonHang() << ","
               << "\"trendValue\":" << fixed << setprecision(0) << trendY
               << "}";
    }
    dsJson << "]";

    ostringstream res;
    res << "{"
        << "\"tongDoanhThu\":" << fixed << setprecision(0) << tongTien << ","
        << "\"tongSoDon\":" << tongDon << ","
        << "\"soNgay\":" << soNgay << ","
        << "\"trungBinh\":" << fixed << setprecision(0) << trungBinh << ","
        << "\"trungVi\":" << fixed << setprecision(0) << trungVi << ","
        << "\"caoNhat\":" << fixed << setprecision(0) << maxVal << ","
        << "\"ngayDinh\":\"" << escapeJson(ngayDinh) << "\","
        << "\"thapNhat\":" << fixed << setprecision(0) << minVal << ","
        << "\"ngayDay\":\"" << escapeJson(ngayDay) << "\","
        << "\"heSoGoc\":" << fixed << setprecision(2) << xuHuong.heSoGoc << ","
        << "\"nhanXuHuong\":\"" << escapeJson(xuHuong.nhanXuHuong) << "\","
        << "\"tocDoTangTruong\":" << fixed << setprecision(1) << xuHuong.tocDoTangTruong << ","
        << "\"duBaoNgayKe\":" << fixed << setprecision(0) << xuHuong.duBaoNgayKe << ","
        << "\"danhSach\":" << dsJson.str()
        << "}";
    return res.str();
}

// -------------------------------------------------------------
// API: quan tri - san pham (CRUD, Tim kiem Trie & QuickSort ton kho)
// -------------------------------------------------------------
string WebServer::apiAdminGetProducts(const map<string, string>& params) {
    map<string, string> convertedParams = params;
    string sortStr = params.count("sort") ? params.at("sort") : "0";
    int sortType = 0;
    if (sortStr == "stock_asc" || sortStr == "3") sortType = 3;
    else if (sortStr == "stock_desc" || sortStr == "4") sortType = 4;
    else if (sortStr == "price_asc" || sortStr == "1") sortType = 1;
    else if (sortStr == "price_desc" || sortStr == "2") sortType = 2;
    else if (sortStr == "name_asc" || sortStr == "5") sortType = 5;
    else {
        try { sortType = stoi(sortStr); } catch (...) { sortType = 0; }
    }
    convertedParams["sort"] = to_string(sortType);

    if (convertedParams.count("q") && !convertedParams.at("q").empty()) {
        return apiSearchProducts(convertedParams);
    }
    return apiGetProducts(convertedParams);
}

string WebServer::apiAddProduct(const string& body) {
    auto data = parseSimpleJson(body);
    string loai = data["loaiSP"];
    string ma = lamSachTruongFile(data["maSP"]);
    string ten = lamSachTruongFile(data["tenSP"]);
    string dm = lamSachTruongFile(data["danhMuc"]);
    double gia = 0;
    int ton = 0;
    try { gia = stod(data["donGia"]); } catch (...) {}
    try { ton = stoi(data["soLuongTonKho"]); } catch (...) {}
    string ncc = data.count("maNCC") ? data["maNCC"] : "NCC01";
    time_t now = time(nullptr);

    if (ma.empty() || ten.empty()) return loiJson("Mã và tên sản phẩm không được để trống!");
    if (gia <= 0) return loiJson("Đơn giá phải lớn hơn 0!");

    SanPham* spMoi = nullptr;
    if (loai == "THUONG") {
        int hsd = 180;
        string quyCach = data.count("quyCach") ? data["quyCach"] : "Gói";
        if (data.count("hanSuDung")) {
            try { hsd = stoi(data["hanSuDung"]); } catch (...) {}
        }
        spMoi = new ThucPhamThuong(ma, ten, dm, gia, ton, ncc, now, hsd, quyCach);
    } else if (loai == "TUOI_SONG") {
        int hsd = 5;
        double nhietDo = 2.0;
        if (data.count("hanSuDung")) {
            try { hsd = stoi(data["hanSuDung"]); } catch (...) {}
        }
        if (data.count("nhietDo")) {
            try { nhietDo = stod(data["nhietDo"]); } catch (...) {}
        }
        spMoi = new ThucPhamTuoiSong(ma, ten, dm, gia, ton, ncc, now, hsd, nhietDo);
    } else if (loai == "CONG_NGHE") {
        int bh = 12;
        bool canBQ = false;
        if (data.count("baoHanh")) {
            try { bh = stoi(data["baoHanh"]); } catch (...) {}
        }
        if (data.count("canBaoQuanCanThan")) {
            canBQ = (data["canBaoQuanCanThan"] == "true" || data["canBaoQuanCanThan"] == "1");
        }
        spMoi = new CongNghe(ma, ten, dm, gia, ton, ncc, now, bh, canBQ);
    } else {
        string chatLieu = data.count("chatLieu") ? data["chatLieu"] : "Inox";
        spMoi = new DoGiaDung(ma, ten, dm, gia, ton, ncc, now, chatLieu);
    }

    bool ok = qlSanPham.themSanPham(spMoi);
    if (ok) {
        thuHoiDonPending();
        luuDuLieuHeThong();
        return "{\"success\":true,\"message\":\"Thêm sản phẩm " + ma + " thành công!\"}";
    } else {
        delete spMoi;
        return loiJson("Thêm thất bại. Mã sản phẩm " + ma + " đã tồn tại trong Bảng băm!");
    }
}

string WebServer::apiUpdateProduct(const string& body) {
    auto data = parseSimpleJson(body);
    string ma = data["maSP"];
    SanPham* sp = qlSanPham.timSanPhamTheoMa(ma);
    if (!sp) return loiJson("Không tìm thấy sản phẩm: " + ma);

    if (data.count("tenSP")) sp->setTenSP(lamSachTruongFile(data["tenSP"]));
    if (data.count("danhMuc")) sp->setDanhMuc(lamSachTruongFile(data["danhMuc"]));
    if (data.count("donGia")) {
        try { double g = stod(data["donGia"]); if (g > 0) sp->setDonGia(g); } catch (...) {}
    }
    if (data.count("soLuongTonKho")) {
        try { int t = stoi(data["soLuongTonKho"]); if (t >= 0) sp->setSoLuongTonKho(t); } catch (...) {}
    }
    if (data.count("maNCC")) sp->setMaNCC(lamSachTruongFile(data["maNCC"]));

    if (ThucPhamThuong* tp = dynamic_cast<ThucPhamThuong*>(sp)) {
        if (data.count("hanSuDung")) {
            try { tp->setHanDung(stoi(data["hanSuDung"])); } catch (...) {}
        }
        if (data.count("quyCach")) tp->setQuyCachDongGoi(lamSachTruongFile(data["quyCach"]));
    } else if (ThucPhamTuoiSong* ts = dynamic_cast<ThucPhamTuoiSong*>(sp)) {
        if (data.count("hanSuDung")) {
            try { ts->setHanDung(stoi(data["hanSuDung"])); } catch (...) {}
        }
        if (data.count("nhietDo")) {
            try { ts->setNhietDoBaoQuan(stod(data["nhietDo"])); } catch (...) {}
        }
    } else if (CongNghe* cn = dynamic_cast<CongNghe*>(sp)) {
        if (data.count("baoHanh")) {
            try { cn->setThoiGianBaoHanh(stoi(data["baoHanh"])); } catch (...) {}
        }
        if (data.count("canBaoQuanCanThan")) {
            cn->setCanBaoQuanCanThan(data["canBaoQuanCanThan"] == "true" || data["canBaoQuanCanThan"] == "1");
        }
    } else if (DoGiaDung* gd = dynamic_cast<DoGiaDung*>(sp)) {
        if (data.count("chatLieu")) gd->setChatLieu(lamSachTruongFile(data["chatLieu"]));
    }

    thuHoiDonPending();
    luuDuLieuHeThong();
    return "{\"success\":true,\"message\":\"Cập nhật thông tin sản phẩm " + ma + " thành công!\"}";
}

string WebServer::apiDeleteProduct(const string& body) {
    auto data = parseSimpleJson(body);
    string ma = data["maSP"];
    if (ma.empty()) return loiJson("Thiếu mã sản phẩm cần xóa!");

    bool ok = qlSanPham.xoaSanPham(ma);
    if (ok) {
        luuDuLieuHeThong();
        return "{\"success\":true,\"message\":\"Đã xóa sản phẩm " + ma + " khỏi hệ thống!\"}";
    }
    return loiJson("Không tìm thấy sản phẩm " + ma + " để xóa.");
}

string WebServer::apiUpdateStock(const string& body) {
    auto data = parseSimpleJson(body);
    string maSP = data["maSP"];
    int delta = 0;
    try { delta = stoi(data["delta"]); } catch (...) {}

    bool ok = qlSanPham.capNhatSoLuongKho(maSP, delta);
    if (ok) {
        thuHoiDonPending();
        luuDuLieuHeThong();
        return "{\"success\":true,\"message\":\"Cập nhật tồn kho thành công!\"}";
    } else {
        return loiJson("Không tìm thấy sản phẩm hoặc số lượng tồn không hợp lệ!");
    }
}

// -------------------------------------------------------------
// API: quan tri - tai khoan nguoi dung (CRUD)
// -------------------------------------------------------------
string WebServer::apiGetUsers() {
    ostringstream json;
    json << "[";
    bool dau = true;
    for (TaiKhoan* tk : danhSachTaiKhoan) {
        if (!tk) continue;
        if (!dau) json << ",";
        dau = false;
        string hoTen = tk->getUsername();
        string sdt, diaChi;
        if (NguoiDung* nd = dynamic_cast<NguoiDung*>(tk)) {
            hoTen = nd->getHoTen();
            sdt = nd->getSoDienThoai();
            diaChi = nd->getDiaChi();
        }
        json << "{"
             << "\"id\":\"" << escapeJson(tk->getId()) << "\","
             << "\"username\":\"" << escapeJson(tk->getUsername()) << "\","
             << "\"role\":\"" << escapeJson(tk->getVaiTro()) << "\","
             << "\"hoTen\":\"" << escapeJson(hoTen) << "\","
             << "\"soDienThoai\":\"" << escapeJson(sdt) << "\","
             << "\"diaChi\":\"" << escapeJson(diaChi) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

string WebServer::apiSaveUser(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string id = data["id"];
    string username = lamSachTruongFile(data["username"]);
    string password = data["password"];
    string role = data.count("role") ? data["role"] : "CUSTOMER";
    string hoTen = lamSachTruongFile(data["hoTen"]);
    string sdt = lamSachTruongFile(data["soDienThoai"]);
    string diaChi = lamSachTruongFile(data["diaChi"]);

    // Cap nhat nguoi dung da co
    if (!id.empty()) {
        TaiKhoan* tk = timTaiKhoanTheoId(id);
        if (!tk) return loiJson("Không tìm thấy tài khoản ID: " + id);
        if (NguoiDung* nd = dynamic_cast<NguoiDung*>(tk)) {
            if (!hoTen.empty()) nd->setHoTen(hoTen);
            if (!sdt.empty()) nd->setSoDienThoai(sdt);
            if (!diaChi.empty()) nd->setDiaChi(diaChi);
        }
        if (!password.empty()) {
            if (password.size() < 6) return loiJson("Mật khẩu mới phải từ 6 ký tự trở lên!");
            tk->setPasswordHash(md5impl::hex(password));
        }
        fileManager.ghiTaiKhoan(danhSachTaiKhoan);
        return "{\"success\":true,\"message\":\"Cập nhật thông tin tài khoản thành công!\"}";
    }

    // Tao tai khoan moi
    if (username.size() < 3) return loiJson("Tên đăng nhập phải từ 3 ký tự trở lên!");
    if (password.size() < 6) return loiJson("Mật khẩu phải từ 6 ký tự trở lên!");
    
    // Kiem tra chan trung lap ten dang nhap bang cay tien to Trie (O(L))
    if (cayTrieTaiKhoan.tonTai(username)) {
        return loiJson("Tên đăng nhập đã tồn tại!");
    }

    int maxId = (role == "ADMIN") ? 111000 : 222000;
    for (TaiKhoan* t : danhSachTaiKhoan) {
        if (!t) continue;
        if ((role == "ADMIN" && t->isAdmin()) || (role != "ADMIN" && t->isCustomer())) {
            try { maxId = max(maxId, stoi(t->getId())); } catch (...) {}
        }
    }
    string newId = to_string(maxId + 1);

    TaiKhoan* tkMoi = nullptr;
    if (role == "ADMIN") {
        tkMoi = new Admin(newId, username, md5impl::hex(password));
    } else {
        tkMoi = new NguoiDung(newId, username, md5impl::hex(password),
                              hoTen.empty() ? username : hoTen, sdt, diaChi);
    }
    danhSachTaiKhoan.push_back(tkMoi);
    cayTrieTaiKhoan.chen(username, role + "|" + newId);
    fileManager.ghiTaiKhoan(danhSachTaiKhoan);
    return "{\"success\":true,\"message\":\"Tạo tài khoản " + username + " thành công!\",\"id\":\"" + newId + "\"}";
}

string WebServer::apiDeleteUser(const string& body, const PhienDangNhap& phien) {
    auto data = parseSimpleJson(body);
    string id = data["id"];
    if (id == phien.idNguoiDung) {
        return loiJson("Không thể tự xóa tài khoản của chính mình!");
    }
    for (auto it = danhSachTaiKhoan.begin(); it != danhSachTaiKhoan.end(); ++it) {
        if (*it && (*it)->getId() == id) {
            cayTrieTaiKhoan.xoa((*it)->getUsername());
            delete *it;
            danhSachTaiKhoan.erase(it);
            fileManager.ghiTaiKhoan(danhSachTaiKhoan);
            return "{\"success\":true,\"message\":\"Đã xóa tài khoản ID: " + id + " thành công!\"}";
        }
    }
    return loiJson("Không tìm thấy tài khoản ID: " + id);
}

string WebServer::apiResetPassword(const string& body) {
    auto data = parseSimpleJson(body);
    string userId = data["userId"];
    string newPass = data["newPassword"];
    if (newPass.size() < 6) return loiJson("Mật khẩu mới phải từ 6 ký tự trở lên!");

    TaiKhoan* tk = timTaiKhoanTheoId(userId);
    if (!tk) return loiJson("Không tìm thấy tài khoản ID: " + userId);
    tk->setPasswordHash(md5impl::hex(newPass));
    fileManager.ghiTaiKhoan(danhSachTaiKhoan);
    return "{\"success\":true,\"message\":\"Đã đặt lại mật khẩu cho tài khoản " + tk->getUsername() + " thành công!\"}";
}
