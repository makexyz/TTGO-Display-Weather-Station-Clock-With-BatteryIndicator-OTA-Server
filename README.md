# TTGO-Display-Weather-Station-Clock-With-BatteryIndicator-OTA-Server

## Về TTGO-Display
TTGO-Display là một board mạch điều khiển nhỏ, có thể được lập trình bằng Arduino IDE. Nó có sẵn khả năng kết nối Wifi, Bluetooth vì sử dụng chip Esp32. Ngoài ra, board mạch này được gắn sẵn một màn hình LCD màu (TFT) có độ phân giải 240x135 đủ để hiển thị khá nhiều thông tin, sáng, đẹp, kể cả trong điều kiện ngoài trời. Nó được thiết kế cho những ứng dụng thông minh cỡ siêu nhỏ và sử dụng nguồn pin nên đã trang bị luôn một jack nguồn JST 1.25mm để gắn pin Lithium. Mình ấn tượng với board này vì nó rất nhỏ, bằng 2 ngón tay thôi nhưng được tích hợp gần như tất cả những thứ mình cần cho các ứng dụng điều khiển từ xa qua Wifi, Bluetooth hay ứng dụng IoT. Và, quan trọng nhất vẫn là, nó rất rẻ. Tại thời điểm mình mua chỉ đâu đó loanh quanh chưa đến 200 ngàn đồng. (Uh, giá thay đổi nhiều rồi nha, chắc do Covid ^^!)

## Về sản phẩm này
Mình mua board này gần nửa năm nhưng gần đây mới có đủ hứng thú để nghịch nó vì công việc chính cũng khá bận. Trước đây, mình cũng từng có thời gian trải nghiệm trên Wemos D1 Mini hay Esp32-CAM về kết nối Wifi hay lưu trữ trên SPIFFS rồi nên thử board này không quá khó. Ứng dụng khiến mình muốn thử đầu tiên là "TTGO T-Display ( ESP32 ) - Internet Weather Station and Clock (tutorial)" của kênh Youtube Volos Projects. Nếu các bạn xem clip bên dưới thì chắc cũng sẽ bị ấn tượng như mình vì thiết kế giao diện cùng animation quá đẹp trên một cái board nhỏ ngang ngửa Arduino Nano. Ngoài ra mình thử cũng vì dự án này opensource. Sản phẩm mình giới thiệu trong bài này dựa hoàn toàn trên code mẫu của clip dưới. Hiện tại mình đã có thể tự thiết kế animation riêng, biết cách hiển thị trên màn hình TFT của TTGO-display nhưng mình quá ưng giao diện nên cũng chẳng muốn làm lại. Thay vào đó, mình muốn mở rộng một số chức năng mà code chính chưa có. Mình sẽ liệt kê ngay bên dưới:

### Chức năng của code mẫu:
1. Lấy thông tin thời tiết từ API của OpenWeather (nhiệt độ, độ ẩm) thông qua kết nối Wifi.
2. Lấy thông tin thời gian thực từ NTP server thông qua kết nối Wifi.
3. Animation trên màn hình khi kết nối Wifi thành công.
4. Hiển thị các thông tin thời gian, thời tiết ra màn hình
5. Tăng / Giảm độ sáng của màn hình bằng nút nhấn tích hợp
6. Invert màu màn hình bằng nút nhấn tích hợp

### Ngoài những chức năng trên, mình bổ sung thêm:
7. Chuyển màn hình mới bằng nút nhấn tích hợp (bỏ tính năng số 6)
8. Đo dung lượng pin hiện tại (được gắn qua jack JST tích hợp của TTGO) và hiển thị ra màn hình (bỏ phần hiển thị SEC - hiển thị giây mặc định)
9. Bộ lọc Kalman để ổn định kết quả đo dung lượng pin.
10. Nạp code từ xa - OTA
11. Máy chủ (Server) đơn giản để theo dõi từ xa xem cái đồng hồ có đang chạy hay không.
12. Modern Web page nhìn xịn xò chứ không phải chỉ 1 dòng text ngắn ngủn theo ví dụ mẫu khi người dùng truy cập.
13. [Dự kiến] Hiển thị số connect đến Server TTGO Display trên màn hình (ảo) số 2 cùng tính năng [7]
14. [Dự kiến] Hiển thị trạng thái các thiết bị được TTGO điều khiển và tương tác trực tiếp với các thiết bị này qua Internet (WebSocket).

> Đừng quên subscribe website và kênh youtube của MakeXYZFun để cập nhật các dự án mới nhất nhé!
* Website: https://makexyz.fun
* Youtube Channel: https://www.youtube.com/channel/UC4v28AauStqzl2rwNGl9QcA
