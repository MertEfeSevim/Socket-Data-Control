using System;
using System.IO;
using System.Net;
using System.Text;
using System.Linq;
using System.Threading;
using System.Net.Sockets;
using System.Diagnostics;
using System.Data.SqlClient;
using System.ComponentModel;
using System.ServiceProcess;
using System.Data.Sql;
using System.Data.SqlServerCe;
using System.Collections.Generic;

namespace AServiceTest
{
    /*      This service provides data transfer from which IP and Port requested.
     *  Datum can be transferred by TCP and UDP methods and can be written to the file.
     *  TCP method can use both IPv4 and IPv6 protocols.
     *  UDP method can use just IPv4.
     *  TCP and UDP can run at the same time because threads have been used.
     *
     *      In the file after writing data or datum to the file,
     *  service writes that data's information down below.
     *  These informations ;    from which port and IP data received,
     *                          Which method used while receiving
     *                          and length of the packet's size.
     *
     *      Two different ports and IP's can be listening at the same time via two different methods,
     *  "clientIP" and "requestedPort" variables requires modification to intended
     *  ports and IP's for TCP and UDP. But this modification has to be done before service starts.
     */

    public partial class Service1 : System.ServiceProcess.ServiceBase
    {
        Thread thTCP;   //Threads for TCP and UDP. So both can start at the same time.
        Thread thUDP;
        //Thread thData;

        bool isRunning = false;

        public Service1() //Initializes Service
        {
            InitializeComponent();
        }
        protected override void OnStart(string[] args)//If Service is running.
        {
            thTCP = new Thread(tcpDataGetter);
            thUDP = new Thread(udpDataGetter);
            //thData = new Thread(database);

            thTCP.Start();
            thUDP.Start();
            // thData.Start();

            isRunning = true;
        }
        private void tcpDataGetter() //TCP Part
        {
            while (isRunning)
            {
                string clientIP = "127.0.0.1"; //Client's IP will be written here
                int requestedPort = (1995);    //Port number here

                TcpListener listenerTCP = new TcpListener(IPAddress.Parse(clientIP), requestedPort);

                /*To connect local host, IP must be used :"127.0.0.1".
                Port number is 1994 in this case. It can be changed.*/

                listenerTCP.Start();
                Socket socketTCP = listenerTCP.AcceptSocket();

                byte[] byteTCP = new byte[byte.MaxValue];
                //Receives package as its size so, there is no need to limitations.

                int byteTCPCount = socketTCP.Receive(byteTCP, SocketFlags.None);

                if (Encoding.UTF8.GetString(byteTCP) == null)
                {
                    File.AppendAllText(@"D:\Tasks\dataGathering.txt", Environment.NewLine + "TCP's Connection:    " + "Disconnected");
                    //If file DNE, it creates file and writes data on file.
                }
                else
                {
                    File.AppendAllText(@"D:\Tasks\dataGathering.txt", Environment.NewLine + "TCP's Data:     " + Encoding.UTF8.GetString(byteTCP) +"TCP was connected to IP:" + clientIP+ " via "+requestedPort+"th port "+ "and incoming Packet's size is :" +(Convert.ToByte(byteTCPCount)));
                    //This line writes information about data.
                }

                socketTCP.Close();
                listenerTCP.Stop();
            }
        }
        private void udpDataGetter() //UDP Part
        {
            while (isRunning)
            {
                string clientIP = "10.0.0.247"; //Client's IP will be written here
                int requestedPort = (1994);     //Port number here

                byte[] data = new byte[byte.MaxValue];
                //Receives package as its size so, there is no need to limitations.
                IPEndPoint endpoint = new IPEndPoint(IPAddress.Parse(clientIP), requestedPort);

                /*To connect local host, IP must be used :"127.0.0.1".
                Port number is 1994 in this case. It can be changed.*/

                Socket newSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                newSocket.Bind(endpoint);

                IPEndPoint sender = new IPEndPoint(IPAddress.Parse(clientIP), requestedPort);
                EndPoint tmpRemote = (EndPoint)sender;

                int recv = newSocket.ReceiveFrom(data, ref tmpRemote);

                if (Encoding.ASCII.GetString(data, 0, recv)==null)
                {
                    File.AppendAllText(@"D:\Tasks\dataGathering.txt", Environment.NewLine + "TCP's Connection:    " + "Disconnected");
                    //If file DNE, it creates file and writes data on file.
                }
                else
                {
                    File.AppendAllText(@"D:\Tasks\dataGathering.txt", Environment.NewLine + "UDP's Data:     " + Encoding.ASCII.GetString(data, 0, recv));
                }
                File.AppendAllText(@"D:\Tasks\dataGathering.txt", Environment.NewLine + "UDP was connected to IP :" + clientIP + " via " + requestedPort + " port" + "and incoming packet's size is " +(Convert.ToByte(recv)));
                //This line writes information about data.

                newSocket.Close();
            }
        }
        /*private void database()
        {
            using (SqlConnection conn = new SqlConnection())
            {
                conn.ConnectionString = "Data Source=.\\SQLEXPRESS;AttachDbFilename=C:\\Users\\efesevim\\Documents\\Visual Studio 2010\\Projects\\AServiceTest\\AServiceTest\\Database1.mdf";
                conn.Open();

                SqlCommand command = new SqlCommand("SELECT * FROM Table2 WHERE toIP = @0",conn);
                command.Parameters.Add(new SqlParameter("0", 127));


                SqlCommand insertCommand = new SqlCommand("INSERT INTO Table2 (toIP, fromIP, dataTCP, dataUDP, date, time, MaxPacketSize) VALUES (@0, @1, @2, @3, @4, @5, @6 ,@7)", conn);

                insertCommand.Parameters.Add(new SqlParameter("0", 1));                 //toIP
                insertCommand.Parameters.Add(new SqlParameter("1", 2));                 //fromIP
                insertCommand.Parameters.Add(new SqlParameter("2", 3));                 //dataTCP
                insertCommand.Parameters.Add(new SqlParameter("3", 2));                 //dataUDP
                insertCommand.Parameters.Add(new SqlParameter("4", DateTime.Today));    //date
                insertCommand.Parameters.Add(new SqlParameter("5", DateTime.Now));      //dateTime
                insertCommand.Parameters.Add(new SqlParameter("6", 2));                 //MaxPacketSize

                conn.Close();
            }
        }*/
        protected override void OnStop() //When service stops
        {
            isRunning = false;

            thTCP = null;
            thUDP = null;
        }
        private void InitializeComponent() //This is what occurs when initialized
        {
            this.CanStop = true;
            this.AutoLog = false;
            this.ServiceName = "AServiceTest";
            this.EventLog.Log = "Application";
            this.EventLog.Source = "Service1";
        }
    }
}