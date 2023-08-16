# AET-6600 16 bit Magnetic Rotary Encoder Interface


`Rotary Encoder ile Açı Okuma (Angle Reading) Kodu`

Bu kod, Raspberry Pi Pico mikrodenetleyicisi üzerinde bir döner enkoder (rotary encoder) kullanarak manyetik bir sensörden açı verisi okumak için yazılmıştır. Sensörün veri iletimi, seri (SSI) iletişimi kullanılarak gerçekleştirilir.

**Özellikler:**

- Manyetik sensör verisini SPI iletişimi kullanarak okuma
- Döner enkoderden gelen veriyi açıya dönüştürme
- Açı verisini belirli bir aralıkta sınırlama
- Sıfır (zero) pozisyonunu ayarlama ve sensör verisini sıfır pozisyonuna göre düzeltme
- Açı verilerini belirli bir süre boyunca döngü içinde okuma ve görüntüleme

**Donanım Bağlantıları:**

- Clock (SCK) pini: 5
- Data (MISO) pini: 6
- Chip Select (CS) pini: 7
- Manyetik Alan Düşük Seviye Pini: 8
- Manyetik Alan Yüksek Seviye Pini: 9
- Programlama Pini: 10
- Mutlak Sıfır Pozisyonu Ayarı Düğmesi: 11

**Fonksiyonlar:**

- `void Setup()`: Pino yönlendirme ve kesme (interrupt) yapılandırmasını yapar.
- `uint16_t ShiftIn(const uint8_t dataPin, const uint8_t clockPin, const uint8_t bitCount)`: Manyetik sensörden veriyi SPI iletişimi kullanarak okur.
- `float ReadPosition()`: Okunan veriyi açıya dönüştürür ve sınırlar.
- `void SetZeroPosition()`: Mutlak sıfır pozisyonunu ayarlar.

**Kullanım:**

1. Pico-Vscode yapılandırması ve SDK kurulumu için [Pico-Vscode](https://github.com/raspberrypi/pico-setup-windows) sayfasını ziyaret edin.
2. Gerekli donanım bağlantılarını yapın.

3. Sensör verisini okuyarak açıyı görüntülemek için `main()` fonksiyonunu kullanın.
4. Sıfır pozisyonunu ayarlamak için "Set Absolute Zero Position" seçeneğini seçin ve düğmeye basın.
5. Sensör verilerini sınırlamak veya düzeltmek için `ShiftIn()` ve `ReadPosition()` fonksiyonlarını kullanın.
6. İstenirse, belirli bir aralıktaki veriyi yakalayarak programlamak için "Program" seçeneğini kullanabilirsiniz.

Bu kod, manyetik sensörden alınan veriyi okuyarak ve işleyerek açı verisi olarak görüntülemek için temel bir yapı sağlar. Daha fazla işlevsellik eklemek veya özelleştirmek için kodu genişletebilirsiniz.
