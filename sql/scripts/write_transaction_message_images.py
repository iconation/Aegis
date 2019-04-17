import mysql.connector # pip install mysql-connector-python
import md5

db = mysql.connector.connect (host="localhost", user="icon", passwd="icon", database="iconation", auth_plugin="mysql_native_password")
cursor = db.cursor()
cursor.execute (open("transaction_message_images.sql", "rb").read())

html = "<html><body>"

for result in cursor.fetchall():
    txid, url, data = result
    html += '<img width="200px" height="200px" src="' + data[2:].decode("hex") + '">'

html += "</body></html>"

open("images.html", "wb+").write(html)