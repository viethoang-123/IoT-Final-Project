**Tên dự án**: Hệ thống giám sát & cảnh báo sử dụng ESP32 + MQTT (EMQX)

 Môn học: Phát triển ứng dụng IoT 
 GV: TS. Nguyễn Anh Tuấn
##  1. Giới thiệu dự án

Dự án xây dựng một hệ thống giám sát và cảnh báo thời gian thực dựa trên vi điều khiển ESP32, kết hợp các cảm biến:

Dòng điện (ACS712)

Nhiệt độ (DS18B20)

Rung động (MPU6050 – tính RMS)

Điện áp đầu vào

Dữ liệu được gửi tới EMQX Cloud MQTT Broker để hiển thị trên dashboard, đồng thời kích hoạt cơ chế cảnh báo & điều khiển relay khi có sự cố.

##  2. Đặt vấn đề

Trong các hệ thống thiết bị điện hoặc mô hình công nghiệp nhỏ, việc giám sát:

Nhiệt độ

Dòng điện bất thường

Dao động mạnh

Điện áp mất ổn định

là vô cùng quan trọng để phát hiện sớm sự cố như:

 - Quá tải
 - Chập điện
 - Rung động nguy hiểm của động cơ
 - Quá nhiệt

Từ đó cần một hệ thống:

Chi phí thấp

Dễ triển khai

Kết nối Internet để giám sát từ xa

Có khả năng tự động cảnh báo & ngắt tải

Đây chính là xuất phát điểm của dự án.
