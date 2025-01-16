/*
 * Created by SharpDevelop.
 * User: bb
 * Date: 8.12.2020
 * Time: 18:22
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace deneme
{
	/// <summary>
	/// Description of MainForm.
	/// </summary>
	public partial class MainForm : Form
	{
		string gelenveri=string.Empty;
		
		public MainForm()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			
			//
			// TODO: Add constructor code after the InitializeComponent() call.
			//
		}
		
		void BtnacClick(object sender, EventArgs e)
		{
			if (!serialPort1.IsOpen){
				serialPort1.PortName=textcom.Text;
				serialPort1.BaudRate=Convert.ToInt32(textBaudrate.Text);
				serialPort1.DataBits=Convert.ToInt32(textdatabit.Text);

				serialPort1.Open();
				serialPort1.DiscardInBuffer();
				serialPort1.DiscardOutBuffer();
				serialPort1.DtrEnable = true;				
			}
		}
		
		void BtnkapatClick(object sender, EventArgs e)
		{
			if(serialPort1.IsOpen){
				serialPort1.Close();
			}
		}
		
		void BtnGonderClick(object sender, EventArgs e)
		{
			serialPort1.Write(textGonderilecekVeriler.Text);
		}
		
		void SerialPort1DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
		{
			gelenveri=serialPort1.ReadLine();
			this.Invoke(new EventHandler(DisplayText));
		}
		
		
		private void DisplayText(object sender, EventArgs e){
			textgelenveriler.Text=textgelenveriler.Text+gelenveri;
			
		}
	
		
		void BtnSaatGonderClick(object sender, EventArgs e)
		{
			serialPort1.Write("%"+textBaslangicSaati.Text+"/");
			// siz bunu bilgisayarın saati şeklinde yapın
		}
		
		void BtnLCD2cisatirisilClick(object sender, EventArgs e)
		{
			
		}
	}
}
