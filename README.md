# Hệ thống giám sát động cơ và cảnh báo sử dụng ESP32 + MQTT 


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
## 3. Kiến trúc tổng thể

Hệ thống được chia thành các phần chính:

- ESP32 chịu trách nhiệm đọc cảm biến và xử lý logic

- Các cảm biến gồm: DS18B20, ACS712, MPU6050, chia áp đo điện áp

- MQTT Broker (EMQX Cloud) dùng để truyền dữ liệu và nhận lệnh

- Dashboard hiển thị và gửi các lệnh điều khiển

- Cơ cấu chấp hành (relay, còi, LED) xử lý khi xảy ra cảnh báo

Sơ đồ hoạt động tóm tắt:
## 4. Sơ đồ phần cứng
Sơ đồ mô phỏng qua fritzing

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/5256b21f-7653-4b00-96e0-a18dac28b76a" />

Sơ đồ đấu nối ESP32 – cảm biến – relay – còi

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/ba38f6ec-6d91-40ef-a274-8cc404c6f7ee" />

Ảnh mô hình thực tế khi đã lắp ráp

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/27e4ae13-0e35-4f21-bad2-028770e34df4" />

## 5. Quy trình thu thập và xử lý dữ liệu

Hệ thống thực hiện:

- Đọc dòng điện qua ACS712

- Đo điện áp đầu vào qua chia áp

- Đo nhiệt độ bằng DS18B20

- Đo gia tốc 3 trục và tính RMS bằng MPU6050

- Gửi dữ liệu 2 giây/lần lên MQTT

Các giá trị sau khi đọc được đưa vào thuật toán kiểm tra ngưỡng:

- Nếu vượt mức → bật cảnh báo, ngắt relay

- Nếu bình thường → hệ thống hoạt động lại

- Tất cả trạng thái cũng được gửi lên MQTT.

## 6. Xử lý MQTT

Hệ thống sử dụng MQTT để truyền dữ liệu và nhận lệnh điều khiển từ dashboard.

* Kết nối MQTT

ESP32 kết nối đến MQTT Broker EMQX thông qua TLS bảo mật.
Các thông tin như host, port, username và password được tách riêng vào namespace để dễ thay đổi.

*  Gửi dữ liệu (Publish)

ESP32 gửi các thông số quan trọng lên MQTT, gồm:

esp32/current — dòng điện

esp32/power — công suất tiêu thụ

esp32/vin — điện áp đầu vào

esp32/temperature — nhiệt độ

esp32/vibration — giá trị RMS rung động

esp32/relay/state — trạng thái relay

Dashboard sẽ đọc các thông tin này để hiển thị cho người dùng.

* Nhận lệnh từ dashboard (Subscribe)

Hệ thống lắng nghe các chủ đề:

esp32/relay/control — bật / tắt relay

esp32/led/control — bật / tắt đèn cảnh báo

esp32/reset/alarm — xóa cảnh báo và kích hoạt lại hệ thống

Khi có lệnh gửi xuống, callback MQTT sẽ phân tích và thực hiện:

Nếu lệnh ON: kích hoạt relay hoặc bật đèn

Nếu lệnh OFF: ngắt relay hoặc tắt đèn

Nếu RESET: xóa toàn bộ trạng thái cảnh báo và khôi phục thiết bị

*  Cơ chế reconnect

Chương trình sử dụng cơ chế tự viết lại để đảm bảo:

Không gây treo vòng lặp (không blocking)

Tự động kết nối lại khi mạng chập chờn

Chỉ reconnect trong khoảng thời gian hợp lý

Nhờ đó, hệ thống hoạt động ổn định ngay cả khi Wi-Fi không tốt.
## 7. Demo

Ảnh dashboard hiển thị dữ liệu: 

- Giao diện hiển thị nhiệt độ

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/6ee44950-fbb8-44f3-980d-bfca4c210ca8" />

- Giao diện hiển thị dòng điện

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/46e784b3-214f-45f4-8730-3aa699506a24" />

- Giao diện hiển thị điện áp 

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/bf428be7-849e-4006-82c5-882ab55bd1ea" />

- Giao diện hiển thị độ rung

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/e847c941-4018-4711-9447-bee779201746" />

- Giao diện hiển thị trạng thái relay và button reset hệ thống khi đã khắc phục xong lỗi và muốn hệ thống hoạt động trở lại

<img width="900" height="525" alt="Image" src="https://github.com/user-attachments/assets/01897558-be9f-49c0-baba-067371b04137" />


Để rõ hơn về hệ thống, mời mọi người theo dõi video demo hệ thống hoạt động dưới đây: 

## 8. Kết quả đạt được

- Thu thập dữ liệu cảm biến ổn định

- Kết nối EMQX hoạt động tốt và ít bị gián đoạn

- Cảnh báo hoạt động đúng chức năng

- Hệ thống có thể giám sát từ xa qua MQTT

- Tuy nhiên do thời gian làm còn ít nên dự án chưa hoàn thành chỉn chu, vẫn còn một vài sai xót như( chưa có hệ cố định cho motor, chưa đóng gói gọn gàng nên dễ gây nguy hiểm cho người thực hiện...) 

## 9. Kết luận

Dự án hoàn thành đúng mục tiêu ban đầu.
Hệ thống hoạt động tốt, có thể áp dụng cho mô hình giám sát động cơ nhỏ hoặc thí nghiệm IoT.

Thời gian sau em sẽ cố gắng phát triển thêm: 

- Bổ sung dashboard web chuyên nghiệp

- Lưu dữ liệu dài hạn bằng InfluxDB / Grafana

- Thêm OTA để cập nhật firmware từ xa

- Tối ưu cảnh báo bằng AI (phân tích rung động nâng cao)
