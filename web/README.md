# Hệ thống quản lý đơn hàng và kho, giao diện mèo đen

Giao diện web cho hệ thống quản lý kho, sản phẩm, đơn hàng và doanh thu. Phong cách dễ thương, chủ đề mèo đen, bảng màu lấy từ ảnh mẫu.

Hai vai trò: **Admin** (quản trị) và **Customer** (khách hàng).

## 1. Công nghệ

| Phần | Công nghệ |
|---|---|
| Giao diện | HTML, CSS, JavaScript thuần |
| Máy chủ | C++17, cpp-httplib, trả dữ liệu JSON |
| Lưu trữ | File `.txt` trong `data/`, không dùng cơ sở dữ liệu |

Không dùng framework CSS (Bootstrap, Tailwind). Không dùng thư viện JavaScript bên ngoài.

## 2. Cấu trúc thư mục

```text
web/
├── index.html    Các màn hình: đăng nhập, khu Admin, khu Customer
├── style.css     Kiểu dáng, bố cục, hiệu ứng mèo, responsive
├── app.js        Gọi API JSON, hiển thị dữ liệu, xử lý form
├── cat.svg       Hình mèo dùng chung (tùy chọn, có thể nhúng trực tiếp trong HTML)
└── README.md     File này
```

## 3. Bảng màu

| Tên | Mã HEX | Dùng cho |
|---|---|---|
| Floral White | `#FFF8F0` | Nền thẻ, nền chữ |
| Jasmine | `#F4D06F` | Nền phụ, thẻ cảnh báo nhẹ, mắt mèo |
| Princeton Orange | `#FF8811` | Màu nhấn, nút hành động |
| Pearl Aqua | `#9DD9D2` | Trạng thái "Đang giao", nền thông tin |
| Space Indigo | `#392F5A` | Sidebar, tiêu đề, chữ chính |

Mỗi màu có thêm một sắc thái nhạt và một sắc thái đậm, khai báo trong `:root`. Không thêm màu ngoài bảng này.

Nền tổng thể là gradient mờ từ vàng nhạt, xanh xám sang cam. Các thẻ nội dung có hiệu ứng kính mờ.

## 4. Các màn hình

| Màn hình | Id | Nội dung |
|---|---|---|
| Đăng nhập | `#view-login` | Mèo đen ngồi phía trên, form tên đăng nhập, mật khẩu, chọn vai trò |
| Khu Admin | `#view-admin` | Sản phẩm, duyệt đơn hàng, kho và cảnh báo, doanh thu |
| Khu Customer | `#view-customer` | Lưới sản phẩm, đặt hàng, đơn của tôi |

Các tab bên trong có id riêng, ví dụ `#admin-products`, `#cust-order`. Lớp `.tab` và `.nav-item` điều khiển việc hiện và ẩn.

Trạng thái đơn hàng và màu tương ứng:

| Trạng thái | Màu |
|---|---|
| Chờ duyệt | Xanh dương nhạt |
| Đang giao | Pearl Aqua |
| Đã hoàn thành | Xanh lá nhạt |
| Đã hủy | Đỏ nhạt |

## 5. Mèo trong giao diện

Mèo đen là nhân vật chính, dùng làm icon, hình minh họa và thông báo.

- **Biến thể:** mèo ngồi, mèo nằm, mèo ôm túi giấy, mèo đội nơ, mèo có đèn Giáng sinh.
- **Vị trí:** mèo ngồi trên màn hình đăng nhập, icon trong sidebar, hình trạng thái rỗng.
- **Trạng thái rỗng:** mèo ôm túi giấy kèm dòng chữ thân thiện, ví dụ "Chưa có đơn nào, mèo đang chờ bạn đặt hàng".
- **Thành công:** mèo đội nơ kèm thông báo ngắn.
- **Lỗi:** mèo nghiêng đầu kèm thông báo rõ ràng.
- **Hoạt ảnh:** chớp mắt định kỳ, đuôi đung đưa khi hover, mắt sáng hơn khi trỏ vào thẻ. Chỉ dùng CSS animation.
- **Nội dung:** không dùng hình ảnh bạo lực hoặc dao, kéo.

Hình mèo vẽ bằng SVG inline hoặc CSS, không tải ảnh ngoài.

## 6. Quy ước HTML và CSS

- Màu và kích thước đặt trong biến `:root`, không viết mã màu trực tiếp trong rule.
- Tên lớp theo chức năng (`.btn-primary`, `.tag-wait`), không theo màu (`.red`).
- Mỗi màn hình là một `<section class="view">`, chỉ một view có lớp `active`.
- Bảng dữ liệu bọc trong `.table-wrap` để cuộn ngang trên điện thoại.
- Số tiền dạng `25.000 đ`, ngày dạng `22/10/2026`.
- Tiêu đề dùng serif đậm, nội dung dùng sans-serif.
- Bo góc 12 đến 16px, khoảng cách rộng.
- Đặt tên lớp và `id` có thể thay đổi nhưng phải báo nhóm trước, vì `app.js` phụ thuộc vào chúng.

## 7. Responsive

- Trên điện thoại (dưới 760px): sidebar chuyển thành thanh ngang cuộn được, bảng cuộn ngang, form một cột.
- Lưới sản phẩm đổi từ nhiều cột xuống hai cột khi màn hình dưới 420px.
- Không có thanh cuộn ngang ở trang chính.

## 8. Kết nối với máy chủ

`app.js` gọi API JSON của `WebServer` (C++). Ví dụ:

```json
{"maSP": "SP001", "tenSP": "Gạo ST25", "donGia": 25000, "soLuongTon": 100}
```

Giao diện không chứa logic nghiệp vụ. Giá, tồn kho, hạn dùng và duyệt đơn do máy chủ tính.

Dữ liệu mẫu trong HTML chỉ dùng khi chưa nối API. Khi nối xong, thay bằng dữ liệu từ máy chủ.

## 9. Nhiệm vụ

| STT | Nhiệm vụ | File | Trạng thái | Người phụ trách |
|---|---|---|---|---|
| 1 | Hoàn thiện khung HTML ba màn hình | `index.html` | Đã có khung | `[điền tên]` |
| 2 | Viết `style.css`: biến màu, nền gradient, kính mờ, sidebar, bảng, nút, thẻ trạng thái, form | `style.css` | Chưa làm | `[điền tên]` |
| 3 | Vẽ mèo bằng SVG hoặc CSS: năm biến thể, icon sidebar, trạng thái rỗng | `index.html`, `style.css` | Chưa làm | `[điền tên]` |
| 4 | Hoạt ảnh mèo: chớp mắt, đuôi đung đưa, hiệu ứng hover | `style.css` | Chưa làm | `[điền tên]` |
| 5 | Responsive theo mục 7 | `style.css` | Chưa làm | `[điền tên]` |
| 6 | Viết `app.js`: gọi API đăng nhập, lấy sản phẩm, đặt hàng, hiển thị đơn | `app.js` | Chưa làm, phụ thuộc `WebServer.cpp` | `[điền tên]` |
| 7 | Thống nhất tên lớp và `id` với người viết `app.js` | `web/` | Chưa làm | `[điền tên]` |

## 10. Yêu cầu nghiệm thu

- Mở `index.html` trực tiếp bằng trình duyệt, không lỗi trong Console.
- Vai trò Admin vào được khu Admin, Customer vào được khu Customer.
- Mọi nút "Đăng xuất" đưa về màn hình đăng nhập.
- Các bảng và form không tràn ra ngoài trên điện thoại.
- Màu chỉ lấy từ bảng màu mục 3.
- Mèo không dùng ảnh tải từ ngoài và không có hình bạo lực.
- Hoạt ảnh mèo tắt được khi người dùng bật chế độ giảm chuyển động (`prefers-reduced-motion`).
- Chạy tốt trên Chrome và Edge, cả desktop và điện thoại.