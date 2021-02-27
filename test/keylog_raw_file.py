import neaky,time,threading

def end_log():
    time.sleep(7)
    neaky.keylog_to_file_stop()

neaky.keylog_to_file(r"C:\Users\warren\Desktop\a.txt")

t = threading.Thread(target=end_log)
t.start()

neaky.message_loop()
