# Giao diện web `web/`

Thư mục này chứa giao diện của hệ thống quản lý đơn hàng và kho, viết bằng HTML và CSS. Giao diện chạy trong trình duyệt và sẽ nhận dữ liệu từ máy chủ C++ (`src/app/WebServer.cpp`) qua các API JSON.

## 1. Cấu trúc file

```text
web/
├── index.html    Cấu trúc các màn hình: đăng nhập, khu Admin, khu Customer
├── style.css     Toàn bộ kiểu dáng, bố cục và responsive
├── app.js        (chưa có) Gọi API, vẽ dữ liệu vào bảng và form
└── README.md     File này
```

## 2. Các màn hình

| Màn hình | Id trong HTML | Nội dung |
|---|---|---|
| Đăng nhập | `#view-login` | Form tên đăng nhập, mật khẩu, chọn vai trò Admin hoặc Customer |
| Khu Admin | `#view-admin` | Sản phẩm, duyệt đơn hàng, kho và cảnh báo, doanh thu |
| Khu Customer | `#view-customer` | Danh sách sản phẩm, đặt hàng, đơn của tôi |

Các tab bên trong mỗi khu có id riêng, ví dụ `#admin-products`, `#cust-order`. Lớp `.tab` và `.nav-item` điều khiển việc hiện và ẩn.

## 3. Quy ước HTML và CSS

- **Đặt màu và kích thước trong biến `:root`** ở đầu `style.css`, không viết mã màu trực tiếp trong từng rule. Ví dụ `--primary`, `--danger`, `--radius`.
- **Đặt tên lớp theo chức năng** (`.btn-primary`, `.tag-wait`), không đặt theo màu (`.red`).
- **Mỗi màn hình là một `<section class="view">`**, chỉ một view có lớp `active` tại một thời điểm.
- **Bảng dữ liệu bọc trong `.table-wrap`** để cuộn ngang trên màn hình nhỏ.
- **Ghi chú ngày, tiền tệ** theo định dạng Việt Nam: `25.000 đ`, `22/10/2026`.
- **Không dùng framework CSS** (Bootstrap, Tailwind) để giữ đơn giản và dễ hiểu cho cả nhóm.

## 4. Nhiệm vụ

| STT | Nhiệm vụ | Trạng thái | Người phụ trách |
|---|---|---|---|
| 1 | Hoàn thành `index.html`: đủ các màn hình trong mục 2 | Đã có khung | `[điền tên]` |
| 2 | Viết `style.css`: biến màu, bố cục sidebar và bảng, nút, thẻ trạng thái, form | Chưa làm | `[điền tên]` |
| 3 | Làm responsive: sidebar thu gọn và lưới sản phẩm đổi số cột trên điện thoại | Chưa làm | `[điền tên]` |
| 4 | Kiểm tra các màn hình trên trình duyệt desktop và điện thoại (Chrome, Edge) | Chưa làm | `[điền tên]` |
| 5 | Thống nhất tên lớp và id với người viết `app.js` trước khi nối API | Chưa làm | `[điền tên]` |
| 6 | Viết `app.js`: gọi API đăng nhập, lấy danh sách sản phẩm, đặt hàng | Chưa làm, phụ thuộc `WebServer.cpp` | `[điền tên]` |

## 5. Yêu cầu nghiệm thu

- Mở `index.html` trực tiếp bằng trình duyệt, không lỗi trong Console.
- Đăng nhập với vai trò Admin vào được khu Admin, chọn vai trò Customer vào được khu Customer.
- Mọi nút "Đăng xuất" đều đưa về màn hình đăng nhập.
- Các bảng không bị tràn ra ngoài màn hình trên điện thoại.
- Mã và tên lớp CSS không trùng với class của thư viện khác.

## 6. Ghi chú

- Dữ liệu trong bảng hiện là **dữ liệu mẫu tĩnh**, sẽ được thay bằng dữ liệu từ API khi `app.js` hoàn thành.
- Trạng thái đơn hàng hiển thị theo lược đồ: `Chờ duyệt`, `Đang giao`, `Đã hoàn thành`, `Đã hủy`.
- Mọi thay đổi về id hoặc tên lớp cần báo nhóm, vì `app.js` phụ thuộc vào chúng.
