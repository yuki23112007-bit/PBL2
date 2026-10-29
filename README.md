# Hệ thống quản lý đơn hàng và kho

Dự án nhóm xây dựng hệ thống quản lý kho, sản phẩm, đơn hàng và doanh thu, với hai phân quyền: **Admin** và **Customer**.

## 1. Quy tắc nhánh (branch)

| Nhánh | Mục đích | Ai được push trực tiếp |
|---|---|---|
| `main` | Code ổn định, chạy được | Không ai. Chỉ merge qua Pull Request |
| `feature/<tên-chức-năng>` | Làm một chức năng | Người phụ trách chức năng đó |

Ví dụ tên nhánh: `feature/dang-nhap-trie`, `feature/bang-bam-san-pham`, `feature/hang-doi-don-hang`.

## 2. Chia việc theo module (tránh xung đột từ đầu)

Mỗi người phụ trách một module, chỉ sửa các file của module đó. Nếu cần sửa file của người khác, phải báo trước trong nhóm.

| Module | Chức năng trong tài liệu | File phụ trách | Người phụ trách |
|---|---|---|---|
| Đăng nhập, đăng ký | Mục 1.1, 2.1 | `models/TaiKhoan.h`, `models/Admin.h`, `models/NguoiDung.h` | `[điền tên]` |
| Tra cứu sản phẩm | Mục 1.2, 2.2 (Trie sản phẩm) | `core/Trie.h` | `Huy` |
| Quản lý sản phẩm, bảng băm | Mục 1.3 | `core/HashTable.h`, `models/SanPham.h`, `models/ThucPham.h`, `models/SanPhamThuong.h`, `models/SanPhamTuoiSong.h`, `models/CongNghe.h`, `models/DoGiaDung.h` | `Huy` |
| Hàng đợi duyệt đơn, Pending List | Mục 1.4, 2.3, 2.7 | `core/OrderQueue.h`, `models/DonHang.h`, `models/ChiTietDonHang.h` | `Huy` |
| Tra cứu đơn hàng | Mục 2.5 | `core/BinarySearchTree.h` (dùng `DonHang.h` của module trên, không sửa) | `Huy` |
| Cảnh báo kho, Min-Heap | Mục 1.6 | `models/KhoHang.h`, `core/MinHeap.h` (cần bổ sung, xem mục 2.1) | `Huy` |
| Doanh thu, báo cáo, vận chuyển | Mục 1.5, 1.7, 2.6 | `models/DoanhThuNgay.h`, `models/VanChuyen.h` | `[điền tên]` |
| Giao diện web (HTML, CSS) | Mục 2.4, 2.5 | `web/index.html`, `web/style.css`, `web/app.js` (xem `web/README.md`) | `[điền tên]` |
| Máy chủ web, API JSON | Mục 2.4, 2.5 | `app/WebServer.h`, `app/WebServer.cpp` | `[điền tên]` |
| Quản lý sản phẩm (nghiệp vụ) | Mục 1.3, 2.2 | `service/QuanLySanPham.h` | `Huy` |
| Quản lý đơn hàng (nghiệp vụ) | Mục 1.4, 2.3, 2.5, 2.7 | `service/QuanLyDonHang.h` | `Huy` |
| Quản lý tài khoản (nghiệp vụ) | Mục 1.1, 2.1 | `service/QuanLyTaiKhoan.h` | `[điền tên]` |
| Quản lý kho và nhà cung cấp | Mục 1.3, 1.6 | `service/QuanLyKhoNCC.h` | `[điền tên]` |
| Quản lý doanh thu (nghiệp vụ) | Mục 1.5, 1.7 | `service/QuanLyDoanhThu.h` | `[điền tên]` |

Mỗi file chỉ xuất hiện ở đúng một dòng. Module khác chỉ `#include` file đó, không sửa.

### 2.1. Cấu trúc thư mục và tên file

```text
src/
├── models/                              Các lớp thực thể nghiệp vụ
│   ├── TaiKhoan.h / TaiKhoan.cpp        Lớp trừu tượng tài khoản (Phân hệ 1)
│   ├── Admin.h / Admin.cpp              Kế thừa TaiKhoan
│   ├── NguoiDung.h / NguoiDung.cpp      Kế thừa TaiKhoan (khách hàng)
│   ├── SanPham.h / SanPham.cpp          Lớp trừu tượng sản phẩm (Phân hệ 2)
│   ├── ThucPham.h / ThucPham.cpp        Kế thừa SanPham (nhóm thực phẩm)
│   ├── SanPhamThuong.h / .cpp           Kế thừa ThucPham
│   ├── SanPhamTuoiSong.h / .cpp         Kế thừa ThucPham
│   ├── CongNghe.h / CongNghe.cpp        Kế thừa SanPham
│   ├── DoGiaDung.h / DoGiaDung.cpp      Kế thừa SanPham
│   ├── DonHang.h / DonHang.cpp          Phân hệ 3
│   ├── ChiTietDonHang.h / .cpp
│   ├── KhoHang.h / KhoHang.cpp
│   ├── NhaCungCap.h / NhaCungCap.cpp
│   ├── VanChuyen.h / VanChuyen.cpp
│   └── DoanhThuNgay.h / DoanhThuNgay.cpp
├── core/                                Cấu trúc dữ liệu và thuật toán (Phân hệ 4)
│   ├── HashTable.h / HashTable.cpp      Bảng băm, O(1)
│   ├── Trie.h / Trie.cpp                Cây tiền tố, O(L)
│   ├── BinarySearchTree.h / .cpp        Cây nhị phân tìm kiếm, O(log N)
│   ├── OrderQueue.h / OrderQueue.cpp    Hàng đợi duyệt đơn FIFO, O(1)
│   ├── MinHeap.h / MinHeap.cpp          Min-Heap cảnh báo hết hạn (mục 1.6, tài liệu chưa có phần này trong Phân hệ 4)
│   └── ThuatToan.h / ThuatToan.cpp      Lớp tiện ích thuật toán (ví dụ QuickSort)
├── service/                             Quản lý nghiệp vụ (Tầng 2 trong tài liệu)
│   ├── QuanLySanPham.h / .cpp           Thêm, sửa, xóa, tìm và sắp xếp sản phẩm
│   ├── QuanLyDonHang.h / .cpp           Đặt hàng, duyệt đơn, lịch sử đơn
│   ├── QuanLyTaiKhoan.h / .cpp          Đăng ký, đăng nhập, quản lý tài khoản
│   ├── QuanLyKhoNCC.h / .cpp            Kho hàng và nhà cung cấp
│   └── QuanLyDoanhThu.h / .cpp          Ghi nhận và báo cáo doanh thu
├── data/                                Đọc, ghi file .txt
│   ├── FileManager.h / FileManager.cpp  Tầng lưu trữ, đọc và ghi 7 file
│   ├── products.txt
│   ├── warehouses.txt
│   ├── suppliers.txt
│   ├── users.txt
│   ├── orders.txt
│   ├── chitietdonhang.txt
│   └── revenues.txt
└── app/                                 Điểm vào chương trình và máy chủ web
    ├── main.cpp                         Hàm main, chỉ khởi tạo và điều phối
    ├── WebServer.h / WebServer.cpp      Máy chủ HTTP (cpp-httplib), trả API JSON cho giao diện web
    └── HeThongQuanLy.h / .cpp           Facade, nạp và lưu dữ liệu khi khởi động và thoát

web/                                     Giao diện HTML và CSS, nằm ngoài src/ (Tầng 3 trong tài liệu)
├── index.html                           Các màn hình: đăng nhập, khu Admin, khu Customer
├── style.css                            Kiểu dáng, bố cục, responsive
├── app.js                               Gọi API và hiển thị dữ liệu (chưa có)
└── README.md                            Quy ước HTML, CSS và nhiệm vụ giao diện
```

**Lưu ý:**
- Mỗi lớp có một cặp `.h` (khai báo) và `.cpp` (cài đặt), cùng tên.
- Năm lớp quản lý (`QuanLySanPham`, `QuanLyDonHang`, `QuanLyTaiKhoan`, `QuanLyKhoNCC`, `QuanLyDoanhThu`) nằm trong `service/`. Đây là tầng nghiệp vụ, gọi `core/`, `data/` và `models/`.
- `HeThongQuanLy` (Facade) nằm trong `app/`, vì nó chỉ điều phối các lớp trong `service/` cho `WebServer`. Không đặt trong `service/` để tránh vòng phụ thuộc với `app/`.
- Giao diện là HTML và CSS trong `web/`, không có giao diện console. Trình duyệt gọi API của `WebServer` qua `app.js`. Phần C++ không đụng đến HTML hay CSS.
- `FileManager` nằm trong `data/` (Tầng 1 trong tài liệu).

**File dùng chung** (như định nghĩa cấu trúc dữ liệu, cấu trúc bảng, hàm băm, hàm tiện ích): ai sửa phải báo cả nhóm trước khi push, vì ảnh hưởng nhiều module.

## 3. Quy trình làm việc hằng ngày

1. Cập nhật code mới nhất:
   ```bash
   git checkout main
   git pull origin main
   ```
2. Tạo nhánh cho chức năng:
   ```bash
   git checkout -b feature/ten-chuc-nang
   ```
3. Làm việc, commit nhỏ và thường xuyên:
   ```bash
   git add .
   git commit -m "Thêm hàm băm cho bảng sản phẩm"
   ```
4. Trước khi push, lấy code mới nhất của `main` về nhánh mình:
   ```bash
   git pull origin main
   ```
5. Đẩy nhánh lên:
   ```bash
   git push -u origin feature/ten-chuc-nang
   ```
6. Tạo **Pull Request** vào `main` trên GitHub. Người còn lại xem qua rồi mới merge.

## 4. Quy ước commit

- Viết tiếng Việt, bắt đầu bằng động từ: "Thêm", "Sửa", "Xóa", "Sửa lỗi".
- Mỗi commit làm một việc. Tránh commit gộp nhiều chức năng.
- Không commit các file sinh ra khi biên dịch (`.o`, `.exe`, thư mục build) hoặc file cá nhân.

## 5. Xử lý xung đột (conflict)

Khi `git pull` báo conflict:

1. Mở file bị conflict, tìm các dấu `<<<<<<<`, `=======`, `>>>>>>>`.
2. Giữ đoạn code đúng, xóa các dấu trên.
3. Lưu file rồi chạy:
   ```bash
   git add <tên-file>
   git commit
   ```

**Không bao giờ** dùng `git push --force` lên `main` hoặc nhánh của người khác.

Nếu hai người cùng sửa một đoạn code, hãy nói chuyện trong nhóm trước khi tự quyết định giữ phần nào.

## 6. Quy tắc khi sửa code

- Không đổi tên hàm hoặc cấu trúc dữ liệu dùng chung mà không báo nhóm.
- Khi thêm chức năng mới, ưu tiên thêm hàm mới thay vì sửa hàm đang chạy của người khác.
- Trước khi tạo Pull Request, đảm bảo code biên dịch và chạy được.
- **Không viết `using namespace std;` trong file `.h`.** Dòng này ảnh hưởng mọi file include header, dễ gây trùng tên với lớp hoặc hàm của nhóm.
- **Trong file `.cpp`, cũng không dùng `using namespace std;`.** Viết tiền tố `std::` (ví dụ `std::string`, `std::cout`). Nếu muốn ngắn hơn, dùng khai báo cụ thể như `using std::string;`.

## 7. Quy tắc tổ chức file: mỗi file một chức năng

- Mỗi file chỉ chứa **một chức năng** hoặc một nhóm chức năng liên quan chặt chẽ (ví dụ: một cấu trúc dữ liệu và các thao tác của nó).
- Không đặt nhiều chức năng khác nhau vào cùng một file, ví dụ không để đăng nhập, tra cứu sản phẩm và thống kê doanh thu chung một file.
- Tên file phải nói rõ chức năng bên trong, ví dụ `bang_bam_san_pham.cpp`, `hang_doi_don_hang.cpp`, không đặt tên chung như `utils.cpp` hay `main_all.cpp`.
- Nếu một file bắt đầu chứa hai chức năng, tách ra thành hai file trước khi tạo Pull Request.
- Hàm dùng chung (tiện ích, kiểm tra dữ liệu) để trong file riêng, không nhét vào file của một module cụ thể.
- Hàm `main` chỉ điều phối luồng chương trình, không chứa logic nghiệp vụ.

**Lý do:** mỗi file thuộc đúng một người phụ trách, nên khi hai người làm việc song song sẽ ít khi sửa cùng một file, và giảm xung đột khi `git pull`.

## 8. Nguyên lý thiết kế SOLID

Áp dụng 5 nguyên lý SOLID khi viết lớp và hàm, để code dễ sửa và ít va chạm khi nhiều người cùng làm.

### S: Single Responsibility (Đơn trách nhiệm)
- Mỗi lớp chỉ có một lý do để thay đổi.
- Ví dụ: lớp `BangBamSanPham` chỉ lưu và tra cứu sản phẩm. Việc ghi file JSON/văn bản tách sang lớp `LuuTruSanPham`.

### O: Open/Closed (Mở để mở rộng, đóng để sửa)
- Thêm hành vi mới bằng cách thêm lớp hoặc hàm mới, không sửa code đang chạy.
- Ví dụ: thêm loại xe giao hàng mới bằng cách thêm một lớp kế thừa, không sửa hàm tính thời gian giao hàng.

### L: Liskov Substitution (Thay thế được)
- Lớp con phải dùng được ở mọi nơi lớp cha được dùng, mà không làm sai logic.
- Ví dụ: nếu `NguoiDungAdmin` và `NguoiDungKhach` kế thừa `NguoiDung`, hàm nhận `NguoiDung` phải chạy đúng với cả hai.

### I: Interface Segregation (Tách giao diện)
- Không ép lớp phải cài đặt hàm nó không dùng. Tách giao diện lớn thành nhiều giao diện nhỏ.
- Ví dụ: tách `QuanLyKho` (thêm, sửa, xóa) khỏi `BaoCaoKho` (xuất báo cáo) nếu một người chỉ cần một trong hai.

### D: Dependency Inversion (Đảo ngược phụ thuộc)
- Module cấp cao không phụ thuộc trực tiếp vào module cấp thấp. Cả hai phụ thuộc vào giao diện (interface, lớp trừu tượng).
- Ví dụ: hàng đợi đơn hàng nhận một đối tượng `KiemTraKho` qua tham số, không tự tạo bằng cách gọi trực tiếp lớp cụ thể. Nhờ đó dễ thay bằng bản giả khi kiểm thử.

**Lưu ý khi làm nhóm:** SOLID liên quan trực tiếp đến quy tắc "mỗi file một chức năng" ở mục 7. Khi tách lớp theo trách nhiệm, mỗi lớp nên có file riêng, và giao diện dùng chung nên được thống nhất trước khi cả nhóm bắt đầu code.

## 9. Liên kết giữa các file

### 9.1. Dùng `#include` để gọi file khác

- Mỗi file `.h` khai báo lớp và hàm. File `.cpp` cài đặt và `#include` file `.h` của chính nó.
- Khi cần lớp khác, file đó `#include` header của lớp đó, ví dụ `#include "../models/SanPham.h"`.
- Mọi header phải có **include guard** để tránh bị include hai lần:

```cpp
#ifndef SANPHAM_H
#define SANPHAM_H
// nội dung
#endif
```

### 9.2. Chiều phụ thuộc (không được vòng lặp)

```text
app / giao diện
      ↓
   service (QuanLy...)
      ↓
core (HashTable, Trie, ...)      data (FileManager)
      ↓                              ↓
   models (SanPham, DonHang, ...)  ←───┘
```

- `models/` không include file của `core/`, `data/`, `service/`, `app/`.
- `core/` chỉ include `models/`.
- `data/` đọc file và tạo đối tượng từ `models/`, không gọi `service/`.
- `service/` ghép `core/`, `data/` và `models/` để thực hiện nghiệp vụ.
- `app/` chỉ gọi `service/`.

**Nếu hai file include nhau:** tách phần chung ra file thứ ba, ví dụ `models/Common.h`, rồi cả hai cùng include file này.

### 9.3. Quy tắc khi làm nhóm

- **Thống nhất header trước, code sau.** Người phụ trách viết xong khai báo trong `.h`, người khác dùng ngay mà không cần đợi phần `.cpp`.
- **Không đổi chữ ký hàm** (tên, tham số, kiểu trả về) khi chưa báo nhóm, vì các file khác đã gọi hàm đó.
- **Đường dẫn `#include` dùng tương đối từ file hiện tại**, không dùng đường dẫn tuyệt đối trên máy cá nhân.
- **Khi thêm file mới**, cập nhật bảng module ở mục 2 và sơ đồ mục 9.2 trong cùng Pull Request.

### 9.4. Biên dịch cả project

Ví dụ `Makefile` đơn giản:

```makefile
SRC = $(wildcard src/*/*.cpp) src/app/main.cpp
OBJ = $(SRC:.cpp=.o)

app: $(OBJ)
	g++ -std=c++17 -o app $(OBJ)

%.o: %.cpp
	g++ -std=c++17 -c $< -o $@
```

Lệnh `make` biên dịch từng `.cpp` thành `.o` rồi liên kết thành file chạy `app`. Đường dẫn `src/app/main.cpp` là giả định, nhóm sửa lại cho đúng cấu trúc thật.

## 10. Quy tắc dùng thư viện có sẵn

Mục tiêu của phần `core/` là thể hiện hiểu biết về cấu trúc dữ liệu và thuật toán, nên phải tự cài đặt.

### 10.1. Phải tự cài đặt (không dùng thư viện có sẵn)

| Thành phần | File | Không được dùng |
|---|---|---|
| Bảng băm | `core/HashTable.h` | `std::unordered_map`, `std::map` |
| Cây tiền tố | `core/Trie.h` | `std::map` cho con trỏ con (dùng `firstChild` và `nextSibling`) |
| Cây nhị phân tìm kiếm | `core/BinarySearchTree.h` | `std::set`, `std::map` |
| Hàng đợi duyệt đơn | `core/OrderQueue.h` | `std::queue`, `std::deque`, `std::list` |
| Min-Heap | `core/MinHeap.h` | `std::priority_queue`, `std::make_heap` |
| Sắp xếp | `core/ThuatToan.h` | `std::sort`, `std::stable_sort`, `std::qsort` |
| Tìm kiếm | `core/ThuatToan.h` | `std::find`, `std::binary_search`, `std::lower_bound` |

Thuật toán QuickSort, duyệt cây, vun đống (upHeap, downHeap) và tìm kiếm nhị phân phải được viết bằng tay.

### 10.2. Được phép dùng

- `std::string`, `std::vector` làm mảng động để lưu con trỏ hoặc bộ đệm, không dùng để thay thế cấu trúc đã tự cài đặt.
- Thư viện nhập xuất, gồm `<iostream>` và `<fstream>`:
  - Các kiểu cơ sở `std::ostream`, `std::istream` dùng làm tham số hoặc kiểu trả về, ví dụ `std::ostream& operator<<(std::ostream& os, const SanPham& sp)`.
  - Các kiểu cụ thể `std::ifstream`, `std::ofstream`, `std::cout`, `std::cin` cho đọc ghi file và giao diện.
- Thư viện chuẩn cho các việc không phải nghiệp vụ, như `<cstdlib>`, `<ctime>` để lấy thời gian `time(NULL)`.

### 10.3. Cách kiểm tra

- Trước khi tạo Pull Request, tìm trong `core/` các từ khóa cấm bằng lệnh:
  ```bash
  grep -rn "std::sort\|std::map\|std::unordered_map\|std::queue\|std::priority_queue\|std::set" src/core/
  ```
- Nếu có kết quả, sửa lại trước khi merge. Người review có quyền từ chối Pull Request vi phạm.

### 10.4. Điểm cần thống nhất trong nhóm

Tài liệu có hai chỗ chưa nhất quán:
- Phần Phân hệ 4 mô tả Trie bằng `map<char, TrieNode*>`, trong khi mục 1.1 yêu cầu `firstChild` và `nextSibling`. **Nhóm đã thống nhất dùng `firstChild` và `nextSibling`**, chi tiết ở mục 11.
- `HashTable` trong Phân hệ 4 dùng `vector<HashNode*> buckets`. Đây là mảng động để giữ con trỏ đầu danh sách, nên được chấp nhận theo mục 10.2. Nhưng nếu nhóm muốn chặt hơn, dùng mảng cấp phát tay như `new HashNode*[capacity]`.

## 11. Thuộc tính TrieNode (theo mục 1.1)

| Thuộc tính | Kiểu | Ý nghĩa |
|---|---|---|
| `kyTu` | `char` | Ký tự mà nút này đại diện |
| `firstChild` | `TrieNode*` | Nút con đầu tiên |
| `nextSibling` | `TrieNode*` | Nút anh em kế tiếp, cùng cha với nút này |
| `isEndOfWord` | `bool` | `true` nếu nút này kết thúc một từ khóa |
| `metadata` | `std::string` | Thông tin gắn với từ khóa. Với tài khoản: `passwordHash`, phân quyền, mã định danh |
| `sanPhamRef` | `SanPham*` | Con trỏ mượn từ HashTable sản phẩm, chỉ dùng khi từ khóa là tên sản phẩm |

**Cách tìm con có ký tự `c`:** bắt đầu từ `firstChild`, duyệt `nextSibling` cho đến khi gặp `kyTu == c`. Thao tác này tốn O(k), với k là số ký tự khác nhau ở cùng một cấp, và không cần `std::map`.

**Lưu ý:** thuộc tính `children` dạng `map` không còn dùng. Mọi thao tác trên Trie (chèn, xóa, tra cứu) phải đi qua `firstChild` và `nextSibling`.

## 12. Liên hệ

Mọi thắc mắc về quy tắc làm việc, hỏi trong nhóm chat trước khi tự sửa file của người khác.
