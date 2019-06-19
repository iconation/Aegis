import mysql.connector # pip install mysql-connector-python

db = mysql.connector.connect (host="localhost", user="icon", passwd="icon", database="iconation")
cursor = db.cursor()
cursor.execute (open("transaction_message_images.sql", "rb").read())

html = "<html><body>"
for txid, url, data in cursor.fetchall():
    html += '<a href="' + url + '"><img width="200px" height="200px" src="' + data[2:].decode("hex") + '"></a>'
html += "</body></html>"

open("images.html", "wb+").write(html)