@echo off
echo ������PN532��ͨ�ż�����������ǰ����ȷ���Ӻõ�Դ�ʹ��ڣ�TXD��RXD���뽻�����ӣ�Ҫ��ʼ�����밴���������
pause
:again
echo ���ڶ���
nfc-list.exe --device PN532_UART:COM7:115200
pause
goto again
