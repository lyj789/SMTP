# SMTP
利用SMTP协议，通过解析MIME文件格式，实现邮件接收系统。
实现一个简单的SMTP服务器，允许标准的客户端，如outlook express发送邮件。
给出协议的交互日志。保存显示客户端发送邮件内容（显示客户端发送的文字和图片）。支持多用户。
一、SMTP协议
（一）基础知识
1、SMTP协议简介
SMTP称为简单邮件传输协议（Simple Mail Transfer Protocal），目标是向用户提供高效、可靠的邮件传输。它的一个重要特点是它能够在传送中接力传送邮件，即邮件可以通过不同网络上的主机接力式传送。SMTP是一个请求/响应协议，它监听25号端口，用于接收用户的Mail请求，并与远端Mail服务器建立SMTP连接。
2、SMTP的连接和发送过程
（a）建立TCP连接
（b）客户端发送HELO命令以标识发件人自己的身份，然后客户端发送MAIL命令；服务器端正希望以OK作为响应，表明准备接收
（c）客户端发送RCPT命令，以标识该电子邮件的计划接收人，可以有多个RCPT行；服务器端则表示是否愿意为收件人接收邮件
（d）协商结束，发送邮件，用命令DATA发送
（e）以.表示结束输入内容一起发送出去
（f）结束此次发送，用QUIT命令退出

用telnet测试给163邮箱发邮件
 

（二）实践
下面根据上述SMTP协议中服务器与客户端的交互过程编写自己的邮件服务器实现与标准客户端的信息交互：
1、环境搭建
利用window自带的邮件客户端，通过新建一个连接到localhost的指定端口的邮箱与Qt编写出的服务器进行交互。SMTP服务器的标准连接端口为25，这里我们在服务器端随意设置了123端口进行监听，只要新建的邮箱连接到服务器端监听的端口，设置成任意端口号均可。
 
下面为开启对指定端口（123）监听的代码：
 
2、连接的初步建立
服务器监听端口，等待客户端与其进行TCP连接。使用标准客户端发送邮件，点击发送按钮时触发newconnection信号，执行槽函数server_new_Connect，利用QTcpserver 的函数nextPendingConnection()获取客户端连接，建立连接。最后客户端通过socket发送220给客户端，回复客户端连接已建立，等待客户端通过socket传递接下来的交互提示信息，代码实现如下：
 
该函数在建立连接的基础上，通过socket中的peerAddress与peerPort函数获取客户端的IP地址与端口号（客户端随机分配端口号与服务器端建立TCP连接）用于服务器端日志的填充。
3、SMTP交互过程
读：
 
写：
 
服务器从缓冲区读出的客户端发送的数据，对其进行判断与处理，并向客户端回传响应码。通过上述两个基本操作，完成服务器端对客户端的通信。代码中使用state用来标识交互进行的阶段。需要指出服务器端对DATA的响应判断，即数据接收部分。代码实现如下：
 
buffer是一个全局变量，this->buffer是本次的接受内容。在实验过程出出现由于DATA内容过多，不能在一个buffer中传递过来的情况，所以需要连续读取buffer直至遇到“\r\n.\r\n”代表数据发送完毕，停止接受数据。再将接受到的this->buffer中的内容添加到textedit中显示出来。接受到的内容就是MIME标准邮件格式。下面将会对其进行具体讲解。
二、MIME文件解析
（一）基础知识
 MIME是目前互联网邮件普遍采用的格式标准，MIME技术规范的完整内容由RFC 2045-2049定义，包括了信息格式、媒体类型、编码方式等各方面的内容。
  MIME邮件的基本信息、格式信息、编码方式等重要内容都记录在邮件内的各种域中，域的基本格式： {域名}：{内容}，域由域名后面跟“：”再加上域的信息内容构成，一条域在邮件中占一行或者多行，域的首行左侧不能有空白字符，比如空格或者制表符，占 用多行的域其后续行则必须以空白字符开头。域的信息内容中还可以包含属性，属性之间以“;”分隔，属性的格式如下：{属性名称}=”{属性值}”。
  邮件规范中定义了大量域，分别用来存储同邮件相关的各种信息，比如发件人的名字和邮件地址信息存储在From域中，收件人的邮件地址信息存储在To域中等。
1、Content-Type域
 在信息提取中最为关键的就是Content-Type域，Content-Type域定义了邮件中所含各种内容的类型以及相关属性。邮件所含的文本、超文 本、附件等信息都按照对应Content-Type域所指定的媒体类型、存储位置、编码方式等信息存储在邮件中。Content-Type域基本格式： Content-Type：{主类型}/{子类型}。
下图为MIME邮件中常见的主类型
 

下面重点解释一下multipart类型：
  MIME邮件中各种不同类型的内容是分段存储的，各个段的排列方式、位置信息都通过Content-Type域的multipart类型来定义。multipart类型主要有三种子类型：mixed、alternative、related。
 ·  multipart/mixed类型
  如果一封邮件中含有附件，那邮件的Content-Type域中必须定义multipart/mixed类型，邮件通过multipart/mixed类型中定义的boundary标识将附件内容同邮件其它内容分成不同的段。
 · multipart/alternative类型
  MIME邮件可以传送超文本内容，但出于兼容性的考虑，一般在发送超文本格式内容的同时会同时发送 一个纯文本内容的副本，如果邮件中同时存在纯文本和超文本内容，则邮件需要在Content-Type域中定义 multipart/alternative类型，邮件通过其boundary中的分段标识将纯文本、超文本和邮件的其它内容分成不同的段。
· multipart/related类型
  MIME邮件中除了可以携带各种附件外，还可以将其它内容以内嵌资源的方式存储在邮件中。比如我们 在发送html格式的邮件内容时，可能使用图像作为html的背景，html文本会被存储在alternative段中，而作为背景的图像则会存储在multipart/related类型定义的段中。
     
multipart的子类型中都定义了各自的boundary属性，邮件使用这些boundary 中定义的字符串作为标识，将邮件内容分成不同的段，段体内的每个子段以“--”+boundary行开始，父段则以“--”+boundary+“--” 行结束，不同段之间用空行分隔。
  
MIME邮件通过多个Content-Type域的multipart类型将内容分成不同的段，这些段在邮件中不是线形顺序排列的，而是存在一个互相包含的层次关系，multipart子类型之间的层次关系结构如下图：
 

2、Content-Transfer-Encoding域
 MIME邮件可以传送图像、声音、视频以及附件，这些非ASCII码的数据都是通过一定的编码规则 进行转换后附着在邮件中进行传递的。编码方式存储在邮件的Content-Transfer-Encoding域中，一封邮件中可能有多个Content -Transfer-Encoding域，分别对应邮件不同部分内容的编码方式。目前MIME邮件中的数据编码普遍采用Base64编码或Quoted- printable编码来实现。下一部分将对邮件收发过程中涉及到的编码方式进行详细解释。

（二）实践
了解了MIME结构之后就要对其进行解析，也就是信息提取。
首先是获取邮件发送方接收方的信息以及邮件主题，通过正则表达式筛选即可实现，代码如下：
 

获取邮件主题时，若主题为中文则需要对其进行quoted_printable解码：
 
邮件正文的分析则比较复杂，需要递归调用MIME_read函数，对每个分块进行相同的处理。划分head和body，对head进行判断，从而决定对body的操作。每一块的具体实现思路是：对获取MIME的content-type，若为multipart类型，则递归调用MIME处理函数，若为text/plain则舍弃；若为text/html则根据content-trabsfer-encoding指定的编码方式（一般为quoted_printable编码）对该区域内容进行解码显示；若为image类型也是根据content-trabsfer-encoding指定的编码方式（一般为base64编码）对该区域内容进行解码并将其加入到html的资源中用于图片的显示。所以该部分实现的关键在于如何定位提取MIME的关键信息，进而划分。我使用了正则表达式QRegularExpression类对MIME进行信息提取。
1、获取边界，分块
 
这里添加了一个boundaryend变量，由于MIME的最后一个boundary边界是以“--”开头“--”结尾的，需要单独区分出来。
 
2、根据每一块head中不同的标志，对body进行不同的处理：
 
继续分块：
 
html：
 
图片：
 

三、编码与解码
（一）基础知识
1、base64
Base64是网络上最常见的用于传输8Bit字节代码的编码方式之一。由于有些网络传送渠道并不支持所有的字节，例如传统的邮件只支持可见字符的传送，像ASCII码的控制字符就不能通过邮件传送。这样就受到了很大的限制，比如图片二进制流的每个字节不可能全部是可见字符，所以就传送不了。最好的方法就是在不改变传统协议的情况下，开辟一种新的方案来支持二进制文件的传送。把不可见字符用可见字符来表示。而Base64就是一种基于64个可见字符来表示二进制数据的表示方法。
加密流程：
第一步，将待转换的字符串每三个字节分为一组，每个字节占8bit，那么共有24个二进制位。
第二步，将上面的24个二进制位每6个一组，共分为4组。
第三步，在每组前面添加两个0，每组由6个变为8个二进制位，总共32个二进制位，即四个字节。
第四步，根据Base64编码对照表获得对应的值。
 
解码过程倒过来即可，具体实现见下面代码。
2、quoted_printable
它是多用途互联网邮件扩展（MIME) 一种实现方式。
编码方法：
任何一个8位的字节值可编码为3个字符：一个等号”=”后跟随两个十六进制数字(0–9或A–F)表示该字节的数值.例如，ASCII码换页符（十进制值为12）可以表示为”=0C”, 等号”=”（十进制值为61）必须表示为”=3D”. 除了可打印ASCII字符与换行符以外，所有字符必须表示为这种格式。

解码方式就是遍历字符串，对每个“=”后面的两个字符，将其替换为ASCII码表中对应的字符。
（二）实践
1、base64
首先设置一个反置base64编码对照表：
 
核心的三个移位操作：
 
2、quoted_printable
 
其中需要注意的是，对等号后面的两个字符进行替换之后，inStr的长度，下次遍历到的字符串会发生错误，所以定义了一个新的INSTR作为该函数的返回值。
注意对i的+2,-1操作。（去掉两个字符串，i+2;等号后面是\r\n不做处理，\n的话 i-1 最后再+2 相当于+1,跳过一个字符）
四、支持多用户
每新建一个连接就新声明一个QTcpSocket *socket，而不是使用一个全局的socket，同时修改socket_Read_Data，MIME_read函数原型，将参数socket传递进去。实现对每个socket的特定操作。
 

 

为了实现每新建一个连接就新增一个button查看按钮，以及一个新的窗体用来显示该邮件内容，我使用了map数据结构，将每个socket与对应生成的继承自MyMainWindow的ui建立映射。再使用connect函数实现每按下button就显示出对应的ui界面。
 

五、最终效果展示
 

 

 

 
