# **Tên dự án**: Hệ thống giám sát & cảnh báo sử dụng ESP32 + MQTT (EMQX)

 Môn học: Phát triển ứng dụng IoT
 
 GV: TS. Nguyễn Anh Tuấn
##  1. Giới thiệu dự án

Dự án xây dựng một hệ thống giám sát và cảnh báo thời gian thực dựa trên vi điều khiển ESP32, kết hợp các cảm biến:

- Dòng điện (ACS712)

- Nhiệt độ (DS18B20)

- Rung động (MPU6050 – tính RMS)

- Điện áp đầu vào

Dữ liệu được gửi tới EMQX Cloud MQTT Broker để hiển thị trên dashboard, đồng thời kích hoạt cơ chế cảnh báo & điều khiển relay khi có sự cố.

##  2. Đặt vấn đề

Trong các hệ thống thiết bị điện hoặc mô hình công nghiệp nhỏ, việc giám sát:

- Nhiệt độ

- Dòng điện bất thường

- Dao động mạnh

- Điện áp mất ổn định

là vô cùng quan trọng để phát hiện sớm sự cố như:

 - Quá tải
 - Chập điện
 - Rung động nguy hiểm của động cơ
 - Quá nhiệt

Từ đó cần một hệ thống:

- Chi phí thấp

- Dễ triển khai

- Kết nối Internet để giám sát từ xa

- Có khả năng tự động cảnh báo & ngắt tải

Đây chính là xuất phát điểm của dự án.
3. Kiến trúc tổng thể

Hệ thống được chia thành các phần chính:

- ESP32 chịu trách nhiệm đọc cảm biến và xử lý logic

- Các cảm biến gồm: DS18B20, ACS712, MPU6050, chia áp đo điện áp

- MQTT Broker (EMQX Cloud) dùng để truyền dữ liệu và nhận lệnh

- Dashboard hiển thị và gửi các lệnh điều khiển

- Cơ cấu chấp hành (relay, còi, LED) xử lý khi xảy ra cảnh báo

Sơ đồ hoạt động tóm tắt:
4. Sơ đồ phần cứng

(Thêm hình vào phần này tùy theo tài liệu của bạn)

Sơ đồ mạch nguyên lý (schematic)

Sơ đồ đấu nối ESP32 – cảm biến – relay – còi

Ảnh mô hình thực tế khi đã lắp ráp

5. Quy trình thu thập và xử lý dữ liệu

Hệ thống thực hiện:

Đọc dòng điện qua ACS712

Đo điện áp đầu vào qua chia áp

Đo nhiệt độ bằng DS18B20

Đo gia tốc 3 trục và tính RMS bằng MPU6050

Gửi dữ liệu 2 giây/lần lên MQTT

Các giá trị sau khi đọc được đưa vào thuật toán kiểm tra ngưỡng:

Nếu vượt mức → bật cảnh báo, ngắt relay

Nếu bình thường → hệ thống hoạt động lại

Tất cả trạng thái cũng được gửi lên MQTT.

6. Xử lý MQTT

Firmware sử dụng kết nối bảo mật (TLS) và các chủ đề (topics) chính:

Publish:

esp32/current

esp32/power

esp32/vin

esp32/temperature

esp32/vibration

esp32/relay/state

Subscribe:

esp32/relay/control

esp32/led/control

esp32/reset/alarm

Phần reconnect MQTT được viết lại để hoạt động ổn định, không làm treo vòng lặp.

7. Demo

(Chỗ này bạn có thể thêm ảnh mô hình và link video nếu có)

Ví dụ:

Ảnh dashboard hiển thị dữ liệu

Ảnh cảnh relay kích hoạt

Video demo hệ thống hoạt động

8. Kết quả đạt được

- Thu thập dữ liệu cảm biến ổn định

- Kết nối EMQX hoạt động tốt và ít bị gián đoạn

- Cảnh báo hoạt động đúng chức năng

- Hệ thống có thể giám sát từ xa qua MQTT

- Độ trễ truyền dữ liệu thấp

9. Kết luận

Dự án hoàn thành đúng mục tiêu ban đầu.
Hệ thống hoạt động tốt, có thể áp dụng cho mô hình giám sát động cơ nhỏ hoặc thí nghiệm IoT.

Hướng phát triển thêm:

- Bổ sung dashboard web chuyên nghiệp

- Lưu dữ liệu dài hạn bằng InfluxDB / Grafana

- Thêm OTA để cập nhật firmware từ xa

- Tối ưu cảnh báo bằng AI (phân tích rung động nâng cao)
