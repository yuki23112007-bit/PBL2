# Thư mục dữ liệu `data/`

Thư mục này chứa 7 file `.txt` làm nơi lưu trữ dữ liệu của hệ thống. Lớp `FileManager` (`src/data/FileManager.h` và `FileManager.cpp`) là nơi duy nhất đọc và ghi các file này.

## 1. Quy ước chung

- **Mỗi bản ghi nằm trên đúng một dòng.**
- **Các trường cách nhau bằng ký tự `|`.** Không đặt `|` trong nội dung trường.
- **Thứ tự trường cố định** như bảng ở mục 2. Ghi và đọc phải dùng cùng thứ tự.
- **Mã hóa UTF-8** để giữ tiếng Việt.
- **Không xuống dòng trong một trường.**
- **Không sửa tay khi chương trình đang chạy.** Chương trình lưu sau mỗi thay đổi (xem mục 4), nên sửa tay khi đang chạy có thể bị ghi đè.
- **Không xóa file này khỏi Git.** Nếu cần dữ liệu mẫu, giữ bản sao trong nhánh riêng.

## 2. Thứ tự các trường trong 7 file

| File | Thứ tự trường | Nguồn |
|---|---|---|
| `users.txt` | Xem mục 3.1 (có hai loại dòng: `ADMIN` và `CUSTOMER`) | Tài liệu mục 1, Phân hệ 1 |
| `products.txt` | Xem mục 3.2 (có hai loại dòng: `THUONG` và `TUOI_SONG`) | Tài liệu mục 1, Phân hệ 2, cộng thêm hai trường theo mục 5 |
| `warehouses.txt` | `maKho\|viTri\|sucChuaToiDa\|maSanPham` | Lược đồ mục I, bảng KhoHang |
| `suppliers.txt` | `maNhaCungCap\|tenCongTy\|diaChi\|soDienThoai\|danhMucCungCap` | Lược đồ mục I, bảng NhaCungCap |
| `orders.txt` | `maDonHang\|tenKhachHang\|diaChiKhachHang\|soDienThoaiKhachHang\|trangThaiDonHang\|ngayDatHang\|IdNguoiDung` | Lược đồ mục I, bảng DonHang |
| `chitietdonhang.txt` | `maChiTiet\|maDonHang\|maSanPham\|soLuong\|thanhTien` | Lược đồ mục I, bảng ChiTietDonHang |
| `revenues.txt` | `ngayThongKe\|tongDoanhThu\|tongSoDonHang` | Lược đồ mục I, bảng DoanhThuNgay |

## 3. Chi tiết và ví dụ

### 3.1. `users.txt`

Hai loại dòng, phân biệt bằng trường đầu tiên:

```text
ADMIN|id|username|passwordHash
CUSTOMER|id|username|passwordHash|hoTen|soDienThoai|diaChi
```

Ví dụ:

```text
ADMIN|1|admin|5f4dcc3b5aa765d61d8327deb882cf99
CUSTOMER|2|an|e10adc3949ba59abbe56e057f20f883e|Nguyễn Văn An|0901234567|Đà Nẵng
```

`passwordHash` là chuỗi băm của mật khẩu, không lưu mật khẩu gốc. Ví dụ trên dùng giá trị mẫu.

### 3.2. `products.txt`

Hai loại dòng. Cả hai đều có `ngaySanXuat` và `hanDungSoNgay` ở cuối (quyết định ở mục 5):

```text
THUONG|maSP|tenSP|danhMuc|donGia|soLuongTon|maNCC|quyCachDongGoi|ngaySanXuat|hanDungSoNgay
TUOI_SONG|maSP|tenSP|danhMuc|donGia|soLuongTon|maNCC|nhietDoBaoQuan|ngaySanXuat|hanDungSoNgay
```

Ví dụ (`ngaySanXuat` là `time_t`, `hanDungSoNgay` là số ngày):

```text
THUONG|SP001|Gạo ST25|Lương thực|25000|100|NCC01|Bao 5kg|1760000000|365
TUOI_SONG|SP002|Cá hồi|Hải sản|250000|20|NCC02|-18|1760400000|7
```

`nhietDoBaoQuan` chỉ có ở dòng `TUOI_SONG`. Hàng khô không có trường này.

### 3.3. `warehouses.txt`

```text
maKho|viTri|sucChuaToiDa|maSanPham
```

Ví dụ:

```text
K01|Kệ A3, tầng 2|500|SP001
```

### 3.4. `suppliers.txt`

```text
maNhaCungCap|tenCongTy|diaChi|soDienThoai|danhMucCungCap
```

Ví dụ:

```text
NCC01|Công ty Gạo Việt|Hà Nội|0241234567|Lương thực
```

### 3.5. `orders.txt`

```text
maDonHang|tenKhachHang|diaChiKhachHang|soDienThoaiKhachHang|trangThaiDonHang|ngayDatHang|IdNguoiDung
```

`ngayDatHang` là `time_t` (số giây từ 1/1/1970).

Ví dụ:

```text
DH001|Nguyễn Văn An|Đà Nẵng|0901234567|Đã duyệt|1761100800|2
```

### 3.6. `chitietdonhang.txt`

```text
maChiTiet|maDonHang|maSanPham|soLuong|thanhTien
```

Ví dụ:

```text
CT001|DH001|SP001|2|50000
```

### 3.7. `revenues.txt`

```text
ngayThongKe|tongDoanhThu|tongSoDonHang
```

`ngayThongKe` là `yyyy-mm-dd` (quyết định ở mục 5).

Ví dụ:

```text
2026-10-22|1250000|12
```

## 4. Lưu dữ liệu sau mỗi thay đổi

Chương trình **không ghi đè file khi thoát**. Mỗi thay đổi (thêm, sửa, xóa, đặt hàng, thanh toán) được lưu ngay xuống file tương ứng. Quy trình:

1. **Ghi vào file tạm trước.** Ví dụ `products.txt.tmp`, ghi xong và đóng file thì mới chuyển sang bước tiếp theo.
2. **Đổi tên file tạm thành file chính** bằng một thao tác đổi tên, nên file `products.txt` luôn là bản cũ hoàn chỉnh hoặc bản mới hoàn chỉnh. Lỗi giữa chừng không làm hỏng file.
3. **Chuyển giữa giao diện Admin và Customer chỉ dùng dữ liệu trong bộ nhớ.** Dữ liệu đã lưu, nên không cần đọc lại file khi đổi giao diện.
4. **Khi khởi động,** đọc các file `.txt` một lần để nạp dữ liệu vào bộ nhớ.

Các quy tắc này áp dụng trong `FileManager` (`src/data/FileManager.cpp`), và `HeThongQuanLy` (`src/app/HeThongQuanLy.cpp`) gọi hàm lưu sau mỗi thao tác.

**Lưu ý:** lưu liên tục làm chậm thao tác nếu file lớn. Nếu thấy chậm, nhóm có thể gom nhiều thay đổi trong một thao tác rồi lưu một lần.

Nếu vẫn muốn sao lưu, thêm vào `.gitignore`:

```text
data/*.tmp
```

## 5. Quyết định đã thống nhất

| Vấn đề | Quyết định | Lý do |
|---|---|---|
| Thiếu `ngaySanXuat` và `hanDungSoNgay` trong `products.txt` | **Thêm vào cả hai loại dòng**, ở cuối dòng | Mục 1.6 của tài liệu tính `NgayHetHan = ngaySanXuat + hanDungSoNgay × 86400` để xếp Min-Heap cảnh báo hết hạn. Thiếu hai trường này thì tính năng đó không chạy được |
| Định dạng ngày | **Mọi mốc thời gian và khoảng thời gian dùng `time_t`** (giây). Riêng `ngayThongKe` dùng `yyyy-mm-dd` vì nó là khóa theo ngày, dễ đọc và dễ tra cứu trong bảng băm | Tài liệu dùng `time_t` cho `ngayDatHang`, `ngayTao`, `thoiGianGiaoDuKien` và công thức hết hạn. Một ngày là một khóa, không cần giây |
| Định dạng 5 file thiếu trong tài liệu | **Dùng thứ tự trường của lược đồ mục I** như mục 2 | Lược đồ là nguồn chính thức duy nhất cho các bảng này |
| `KhoHang` chỉ có một `maSanPham` | **Giữ theo lược đồ**: mỗi kho chứa một sản phẩm | Đúng với thiết kế trong tài liệu. Nếu một kho cần chứa nhiều sản phẩm, đổi khóa thành cặp `(maKho, maSanPham)` |

### 5.1. Ảnh hưởng đến `models/`

Các thay đổi trên làm thay đổi các lớp sau:

- **`SanPham`** (lớp trừu tượng, `models/SanPham.h`): thêm thuộc tính `ngaySanXuat` (`time_t`) và `hanDungSoNgay` (`int`), và thêm phương thức `ngayHetHan()` trả về `ngaySanXuat + hanDungSoNgay * 86400`. Phương thức này được `core/MinHeap` dùng làm khóa.
- **`SanPhamThuong`** và **`SanPhamTuoiSong`**: hàm tạo nhận thêm hai tham số. Hàm `xuatChuoiFile()` của cả hai xuất thêm hai trường ở cuối dòng.
- **`DoanhThuNgay`** (`models/DoanhThuNgay.h`): thuộc tính `ngayThongKe` đổi kiểu thành `std::string` để làm khóa.
- **`KhoHang`** (`models/KhoHang.h`): không đổi, vì giữ theo lược đồ.
- **`NguoiDung`** (`models/NguoiDung.h`): không lưu danh sách mã đơn trong `users.txt`. Khi khởi động, danh sách này được dựng lại từ `orders.txt` theo trường `IdNguoiDung`.

Các lớp `TaiKhoan`, `Admin`, `DonHang`, `ChiTietDonHang`, `NhaCungCap`, `VanChuyen` không đổi.

**Lưu ý khi làm nhóm:** đổi hàm tạo của `SanPham` và các lớp con là đổi chữ ký hàm, nên phải báo cả nhóm trước khi push (mục 9.3 của README chính).

## 6. Chọn bộ dữ liệu: thật hay test

Có hai thư mục dữ liệu cùng cấu trúc 7 file:

| Thư mục | Dùng khi | Ghi chú |
|---|---|---|
| `data/` | Chạy thật, làm báo cáo | Dữ liệu thật. Không dùng để test |
| `data_test/` | Test, thử chức năng | Bản sao của `data/`, có thể sửa, xóa, nạp sai thoải mái |

Chương trình **không ghi cứng đường dẫn** trong code. `FileManager` nhận đường dẫn gốc qua hàm tạo, và `main.cpp` chọn thư mục theo tham số dòng lệnh:

```cpp
// src/app/main.cpp
int main(int argc, char* argv[]) {
    std::string dataDir = "data";
    if (argc > 1 && std::string(argv[1]) == "--test") {
        dataDir = "data_test";
    }
    HeThongQuanLy hệThống(dataDir);   // truyền xuống FileManager và WebServer
    hệThống.chạy();
    return 0;
}
```

Cách chạy:

```text
./app              # dùng data/ (dữ liệu thật)
./app --test       # dùng data_test/ (dữ liệu test)
```

Trong Makefile có hai đích tương ứng:

```makefile
run:
	./app

test-run:
	./app --test
```

**Thiết lập lần đầu:** chạy `cp -r data data_test` một lần để tạo bộ test. Sau đó nên commit `data_test/` vào Git để cả nhóm dùng chung một bộ test. Không commit `data/` nếu dữ liệu thật không được công khai.

**Quy tắc:** khi test, không chạy `./app` với `data/` đang giữ dữ liệu báo cáo, vì chương trình lưu sau mỗi thay đổi và sẽ ghi trực tiếp vào `data/`. Muốn thử lại bộ test từ đầu thì chép lại từ `data/` hoặc từ bản commit của `data_test/`.
