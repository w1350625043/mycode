using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Net;
using System.IO;
namespace TCP_Connection
{
    public partial class Form1 : Form
    {
        private TcpClient myTcpClient = null;
        
        Thread ConnectThread;

        string ipAddress;
        int Port = 0;

        string path = null;

        private NetworkStream networkStream = null;
        private Thread ReceiveThread;

        public Form1()
        {
            InitializeComponent();
          

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        private void ConnectMethod()
        {
            myTcpClient = new TcpClient();
            try
            {
                myTcpClient.Connect(ipAddress, Port);

                networkStream = myTcpClient.GetStream();
                ReceiveThread = new Thread(ReceiveDataMethod);

                ReceiveThread.Start();
                try
                {
                    networkStream.Write(new byte[] { 0x31, 0x32, 0x33 }, 0, 3);
                }
                catch 
                {
                
                }
                Invoke((new Action(() =>
                {
                    button1.Text = "断开";
                })));

            }
            catch (Exception)
            {
                Invoke((new Action(() =>
                {
                    button1.Text = "连接";
                })));
                try { myTcpClient.Close(); } catch { }
            }
        }
        private void getIPAddress()
        {
            IPAddress[] hostipspool = Dns.GetHostAddresses("");
            comboBox1.Items.Clear();
            foreach (IPAddress ipa in hostipspool)
            {
                if (ipa.AddressFamily == AddressFamily.InterNetwork)
                {
                    comboBox1.Items.Add(ipa.ToString());
                    comboBox1.SelectedIndex = comboBox1.Items.Count > 0 ? 0 : -1;
                }
            }

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (button1.Text == "连接")
            {
                ipAddress = comboBox1.Text.ToString();
                Port = Convert.ToInt32(textBox1.Text.ToString());
                ConnectThread = new Thread(ConnectMethod);
                ConnectThread.Start();

                StreamWriter sw = new StreamWriter(path, true);

                if (!(comboBox1.Items.Contains(comboBox1.Text)))
                {

                    sw.WriteLine(comboBox1.Text);

                }
                comboBox1.Items.Insert(0, comboBox1.Text);

                sw.Close();
            }
            else
            {
                try { myTcpClient.Close(); } catch { }
                Invoke((new Action(() =>
                {
                    button1.Text = "连接";
                })));
            }
        }

        private void flowLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged_1(object sender, EventArgs e)
        {

        }

        private void comboBox1_DropDown(object sender, EventArgs e)
        {
            getIPAddress();
            path = @"d:1.ini";
            if (File.Exists(path))
            {
                StreamReader sr = new StreamReader(path, true);
                while (sr.Peek() > 0)
                {
                    comboBox1.Items.Add(sr.ReadLine());
                }
                sr.Close();
                comboBox1.Text = (string)comboBox1.Items[0];
            }
            
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            getIPAddress();
        }
        private string GetTimeStr()
        {
            /* 获取当前时间字符串函数 */
            string tempTimeStr = DateTime.Now.Year.ToString() + "-" + DateTime.Now.Month.ToString().PadLeft(2, '0')
                        + "-" + DateTime.Now.Day.ToString().PadLeft(2, '0') + "  " + DateTime.Now.Hour.ToString().PadLeft(2, '0')
                        + ":" + DateTime.Now.Minute.ToString().PadLeft(2, '0') + ":" + DateTime.Now.Second.ToString().PadLeft(2, '0')
                        + ":" + DateTime.Now.Millisecond.ToString().PadLeft(2, '0'); /* 以年-月-日时分秒的格式命名文件 */
            return tempTimeStr;
        }

        private void ReceiveDataMethod()
        {
            int RecvCnt = 0;
            byte[] recvBytes = new byte[1024];
            while (true)
            {
                try
                {   if (myTcpClient.Client.Poll(20, SelectMode.SelectRead) && (myTcpClient.Client.Available == 0))
                    {
                        myTcpClient.Close();
                    }
                    RecvCnt = networkStream.Read(recvBytes,0,recvBytes.Length);
                    Invoke((new Action(() =>
                        {
                            DateTime t = System.DateTime.Now;
                          
                            textBox2.AppendText(GetTimeStr());
                            textBox2.AppendText("\r\n");
                            textBox2.AppendText(new ASCIIEncoding().GetString(recvBytes,0,RecvCnt));
                        textBox2.AppendText("\r\n");
                        })));
                }
                catch (Exception ex)
                {
                    Invoke((new Action(() =>
                    {
                        button1.Text = "连接";
                    })));
                    try { ReceiveThread.Abort(); }
                    catch { }
                    try { networkStream.Dispose(); }
                    catch { }
                    try { myTcpClient.Close(); }
                    catch { }
                }
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
           
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            try { ReceiveThread.Abort(); } catch { }
            try { networkStream.Dispose(); } catch { }
            try { myTcpClient.Close(); } catch { }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            byte[] sendbyte = Encoding.Default.GetBytes(textBox3.Text.ToString());
            if (sendbyte.Length > 0)
            {
                try { networkStream.Write(sendbyte, 0, sendbyte.Length); }
                catch (Exception) { MessageBox.Show("请检查连接","提示！"); }
            }
            else
            { MessageBox.Show("数据不能为空","提示"); }
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            textBox2.Clear();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            textBox3.Clear();
        }

        private void comboBox2_DropDown(object sender, EventArgs e)
        {

        }
    }
}
