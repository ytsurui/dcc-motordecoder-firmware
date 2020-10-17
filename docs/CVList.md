# CV List

<table>
  <tr>
    <td>CV番号</td>
    <td>初期値</td>
    <td>内容</td>
  </tr>
  <tr>
    <td>1</td>
    <td>3</td>
    <td>2桁アドレス (7-bit Address)</td>
  </tr>
  <tr>
    <td>2</td>
    <td>0</td>
    <td>始動電圧 (0-255)</td>
  </tr>
  <tr>
    <td>3</td>
    <td>0</td>
    <td>加速度 (0-31)</td>
  </tr>
  <tr>
    <td>4</td>
    <td>0</td>
    <td>減速度 (0-31)</td>
  </tr>
  <tr>
    <td>5</td>
    <td>255</td>
    <td>最大電圧 (0-255)</td>
  </tr>
  <tr>
    <td>6</td>
    <td>127</td>
    <td>中間電圧 (0-255)</td>
  </tr>
  <tr>
    <td>7</td>
    <td>1</td>
    <td>バージョン番号</td>
  </tr>
  <tr>
    <td>8</td>
    <td>13</td>
    <td>メーカーID</td>
  </tr>
  <tr>
    <td>15</td>
    <td>0</td>
    <td>デコーダロック番号</td>
  </tr>
  <tr>
    <td>16</td>
    <td>0</td>
    <td>デコーダロック番号</td>
  </tr>
  <tr>
    <td>17</td>
    <td>192</td>
    <td>4桁アドレス下位 (14-bit Address)</td>
  </tr>
  <tr>
    <td>18</td>
    <td>0</td>
    <td>4桁アドレス上位 (14-bit Address)</td>
  </tr>
  <tr>
    <td>19</td>
    <td>0</td>
    <td>Consist Address </td>
  </tr>
  <tr>
    <td>29</td>
    <td>6</td>
    <td>デコーダ基本設定</td>
  </tr>
  <tr>
    <td>47</td>
    <td>202</td>
    <td>キックスタート
    <br />Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable</td>
  </tr>
  <tr>
    <td>56</td>
    <td>4</td>
    <td>PWM周波数設定
    <br />ATTiny45 TCCR1 CS13-CS10 Configuration RAW Data</td>
  </tr>
  <tr>
    <td>58</td>
    <td>127</td>
    <td>アナログ走行時の最大速度 (0-255)</td>
  </tr>
  <tr>
    <td>59</td>
    <td>3</td>
    <td>アナログ走行時の加減速度</td>
  </tr>
  <tr>
    <td>67-94</td>
    <td>-</td>
    <td>スピードテーブル (14step / 28step)</td> 
  </tr>
</table>