# Simple Dual-Axis Solar Tracker

# Đề Tài: Mô Hình Bộ Điều Hướng Pin Mặt Trời

## 1. Đặt vấn đề

Đối với hầu hết các trường hợp, các tấm pin mặt trời phổ biến hàng ngày của chúng ta chạy với hiệu suất 18-20%, nghĩa là chúng chỉ chuyển đổi 18-20% lượng ánh sáng mặt trời nhận được thành điện năng.

Bài toán đặt ra là chúng ta cần cải thiện hiệu quả của các tấm pin hoặc tìm cách nhận được nhiều ánh sáng hơn.

Cách đơn giản nhất để tối ưu hóa năng lượng là điều chỉnh tấm pin hướng về mặt trời giống như cách hoa hướng dương hoạt động.

Trên thực tế, các tấm pin mặt trời có cơ chế theo dõi mặt trời tạo ra nhiều hơn khoảng 30% năng lượng mỗi ngày so với tấm pin cố định.

Dự án "Simple Dual-Axis Solar Tracker" được triển khai nhằm giải quyết vấn đề trên bằng cách sử dụng vi điều khiển ESP32, các cảm biến ánh sáng (LDR), và giao thức MQTT để điều khiển từ xa qua giao diện Node-RED Dashboard. Hệ thống này không chỉ cung cấp chế độ tự động mà còn cho phép điều khiển thủ công, mang lại sự linh hoạt và hiệu quả cao cho người dùng.

---

## 2. Cấu tạo

Hệ thống theo dõi mặt trời được cấu tạo bởi:

- **ESP32 DOIT Devkit V1**: Bộ điều khiển chính, có nhiệm vụ xử lý tín hiệu và điều khiển động cơ.
- **Cảm biến LDR (Light Dependent Resistor)**: Để đo cường độ ánh sáng từ các hướng khác nhau.
- **Động cơ Servo**:
  - 1 động cơ servo chuyển động ngang (theo trục azimuth).
  - 1 động cơ servo chuyển động dọc (theo trục elevation).

---

## 3. Nguyên lý hoạt động

Hệ thống hoạt động dựa trên sự chênh lệch cường độ ánh sáng nhận được từ các cảm biến LDR. Tín hiệu từ các cảm biến được gửi đến ESP32 để xử lý. Dựa vào sự chênh lệch ánh sáng:

- **Chế độ tự động**:
  - Động cơ servo sẽ xoay để tấm pin mặt trời hướng đến nơi có ánh sáng mạnh nhất.
- **Chế độ thủ công**:
  - Người dùng có thể điều khiển động cơ thông qua giao diện Node-RED Dashboard để điều chỉnh tấm pin theo ý muốn.

---

## Tính năng
- **Chế độ tự động**: Điều chỉnh vị trí tấm pin dựa trên cường độ ánh sáng được phát hiện bởi cảm biến LDR.
- **Chế độ thủ công**: Cho phép điều khiển động cơ ngang và dọc thông qua Node-RED Dashboard.
- **Tích hợp MQTT**: Giao tiếp với HiveMQ để gửi dữ liệu cường độ ánh sáng và nhận lệnh điều khiển.
- **Node-RED Dashboard**: Cung cấp giao diện tương tác để chuyển đổi chế độ và điều khiển động cơ.

---

## Thành phần
- **Phần cứng**:
  - ESP32
  - 4 x Cảm biến ánh sáng (LDR)
  - 2 x Động cơ Servo (SG90 hoặc tương đương)
  - Điện trở (10kΩ) cho mạch LDR
  - Nguồn cấp (5V, khuyến nghị 2A cho động cơ servo)
- **Phần mềm**:
  - PlatformIO
  - Node-RED với MQTT broker: HiveMQ

---

**Sơ đồ khối:**
![block_diagram](so_do_khoi.png)

---

**Sơ đồ kết nối phần cứng:**
![Fritzing Diagram](images/fritzing.png)

---

**Giao diện Dashboard:**
![Node-RED Dashboard](images/dashboard_UI.png)

---

### 3. **Chế độ điều khiển**
- **Chế độ tự động**:
  - Mặc định là chế độ tự động.
  - Điều chỉnh góc servo tự động dựa trên giá trị LDR.
- **Chế độ thủ công**:
  - Sử dụng các nút trên Node-RED để điều khiển servo thủ công.

---

**Luồng Node-RED:**
![Node-RED Flow](node-red_flow.png)

---

## MQTT Topics
- **Publish**:
  - `solar/light`: Gửi dữ liệu cường độ ánh sáng.
- **Subscribe**:
  - `solar/control`: Nhận lệnh điều khiển.

#### Lệnh ví dụ:
- `AUTO_ON`: Bật chế độ tự động.
- `AUTO_OFF`: Tắt chế độ tự động.
- `MANUAL_HORIZONTAL:UP`: Di chuyển động cơ ngang lên.
- `MANUAL_HORIZONTAL:DOWN`: Di chuyển động cơ ngang xuống.
- `MANUAL_VERTICAL:UP`: Di chuyển động cơ dọc lên.
- `MANUAL_VERTICAL:DOWN`: Di chuyển động cơ dọc xuống.

---

## Kiểm thử
**Video kiểm thử hệ thống:**
[Link Video](videos/kiemthu.mp4)

---

## Hướng phát triển

Dưới đây là một số hướng phát triển tiềm năng cho dự án:

1. **Tích hợp pin mặt trời và pin lưu trữ**:
   - Lắp thêm pin mặt trời để thu năng lượng thực tế.
   - Sử dụng pin Li-ion để lưu trữ năng lượng, cung cấp điện cho hệ thống khi không có ánh sáng mặt trời.

2. **Tối ưu hóa thuật toán điều hướng**:
   - Sử dụng các thuật toán nâng cao như PID để cải thiện độ chính xác của hệ thống.

3. **Tích hợp cảm biến bổ sung**:
   - Cảm biến nhiệt độ và độ ẩm để theo dõi môi trường.
   - Cảm biến gió để bảo vệ tấm pin khỏi hư hỏng trong điều kiện thời tiết khắc nghiệt.
