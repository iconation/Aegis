<p align="center">
  <img 
    src="https://iconation.team/images/very_small.png" 
    width="120px"
    alt="ICONation logo">
</p>

<h1 align="center">Aegis - ICON blockchain analyzer</h1>

 [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

# What is Aegis?

Basically, Aegis is the ICON blockchain stored into a MySQL database. It lets you query quickly and efficiently any information from the ICON blockchain, for analysis purposes. Or just because being curious is fun.

# How Can I Use It?

Aegis is designed for developers with prior SQL experience. If you don’t know SQL queries at all, you won’t be able to use Aegis. However, as Aegis SQL schema is quite simple, it isn’t hard to craft simple queries that retrieve interesting results.

Refer to the Aegis SQL Schema for more details : [sql/schema](sql/schema)

# Installing Aegis

Want to try Aegis? Here are some simple steps for using Aegis:

- Download and install **MySQL Server + MySQL Workbench**: https://dev.mysql.com/downloads/installer/
- Create a new SQL user called **icon**, with the password **icon**.
- Download Aegis: https://github.com/iconation/aegis/releases/latest (if you’re not on Windows, sorry, you’ll need to compile it)
- Please download both the executable and the SQL Schema (**Aegis.SQL.Schema.zip**) in the previous link!
- Extract **Aegis.SQL.Schema.zip** anywhere on your disk.
- Launch MySQL WorkBench, connect to your MySQL Server and create a new schema called **iconation** using the Navigator window on the left.
- Imports the Aegis Schema using the contextual menu: Server > Data Import > Import from Dump Project Folder > Select the folder where you extracted the SQL Schema > Load Folder Contents > Start Import.
![https://cdn-images-1.medium.com/max/800/0*dxt4Db3mecu5AaD9.gif](https://cdn-images-1.medium.com/max/800/0*dxt4Db3mecu5AaD9.gif)
- You’re now ready to use Aegis! You can now launch it anytime, and it will start filling your Aegis database. Depending on the Citizen node you’re using and your computer performance, it can take a few hours to download the entire blockchain and fill your database.

![https://cdn-images-1.medium.com/max/800/0*eddnhqV8yjU8khQ9.gif](https://cdn-images-1.medium.com/max/800/0*eddnhqV8yjU8khQ9.gif)
Aegis is downloading and inserting the ICON blocks at the same time.

Please refer to the Aegis schema if you’re looking for information about what data the Aegis Database contains, the fields should be very straight-forward to understand.

If you have questions or are interested in more, please contact us using the contact form on the [ICONation website](https://iconation.team/) or join [our Telegram](https://t.me/iconationteam).
