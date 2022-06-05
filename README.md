# RasPico_SHIBAKI_Board
Raspberry Pi Pico で色々やる基板です。  

### 搭載インタフェース
| インタフェース | 説明 |
| ---- | ---- |
| SFP | 2スロット。TXRX、LOS、TX_FAULT、I2Cの基本的な信号線がPicoと接続されています。RATESEL、TXDISABLEはんだジャンパで指定します。 |
| GPIO | 2.54mm pitch 2x5ピンヘッダ。4pin + 3.3V + GND が出ています。機能拡張等に使用できます。 |
| LED | 4個搭載（Picoボードに標準搭載のLEDを含めると合計5個）。LチカデバッグやUART、S/PDIF信号を出力して遊べます。 |
| Push SW | User用2個（プルアップ抵抗付き）、Reset用1個 |
| SMA Input | 2個搭載。50Ω終端抵抗を実装するパッドを用意しています。 |
| SMA Output | 2個搭載。標準で450Ωの抵抗が直列に挿入されています。50Ω終端可能なオシロスコープに接続するとx10減衰として機能します。レベルを変更した場合は適宜抵抗を打ち替えてください。 |

### 基板画像
<img src="doc/pcb_top.png" width="500">  
<img src="doc/pcb_bottom.png" width="500">  
<img src="doc/pcb_size.png" width="500">  

## サンプルソフトウェア（準備中・・・）
本ボードで遊べるサンプルプログラムです。  
100BASE-FXによるUDP送信例、VBANプロトコルによるPCMデータ送信、SFPモジュール情報読み出し、8b10bエンコード・デコード（PIOを使ったSerDes with Clockrecovery）等を追々追加していく予定です。  

## 回路図
[Schematic.pdf](doc/schematic.pdf "Schematic")

## 開発環境
- KiCad Version 6.0.5
- Raspberry Pi Pico 3D model and Footprint : [KiCad-RP-Pico](https://github.com/ncarandini/KiCad-RP-Pico "KiCad-RP-Pico")
