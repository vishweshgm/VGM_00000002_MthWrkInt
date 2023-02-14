using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
namespace Voltage_Divider_Calculator
{
    public partial class Form1 : Form
    {
        public float R1 = 1000;
        public float R2 = 1000;
        public float Vout = 5;
        public float Vcc = 10;
        public float I = (float)0.005;
        public float pR1 = (float)0.025;
        public float pR2 = (float)0.025;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            DateTime today = DateTime.Today;
            DateTime EndDate = new DateTime(2023, 12, 31);
            if (today.Date > EndDate.Date) {
                MessageBox.Show("It is time to upgrade the software\nContact vishweshgm@outlook.com", "Creator", MessageBoxButtons.OK, MessageBoxIcon.Information);
                Application.Exit();
            }
        }

        private void btnTest_Click(object sender, EventArgs e)
        {

        }

        private void txtBoxVcc_TextChanged(object sender, EventArgs e)
        {





        }

        private void txtBoxR1_TextChanged(object sender, EventArgs e)
        {


        }

        private void txtBoxR2_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtBoxVout_TextChanged(object sender, EventArgs e)
        {

        }

        private void Form1_MouseHover(object sender, EventArgs e)
        {

            Graphics surface = CreateGraphics();
            Pen pen1 = new Pen(Color.Black, 2);
            Pen pen2 = new Pen(Color.Black, 2);
            Pen pen3 = new Pen(Color.Black, 2);
            Pen pen4 = new Pen(Color.Black, 2);
            Pen pen5 = new Pen(Color.Black, 2);

            surface.DrawLine(pen1, 60, 135, 670, 135);
            surface.DrawLine(pen2, 670, 135, 670, 200);
            surface.DrawLine(pen2, 375, 135, 375, 300);
        }

        private void btnVcc_Click(object sender, EventArgs e)
        {
            string resultString;
            try
            {
                if ((txtBoxR1.Text != "") &&
                    (txtBoxR2.Text != "") &&
                    (txtBoxVout.Text) != "")
                {
                    resultString = Regex.Match(txtBoxR1.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxR1.Text, @"\d+").Value;
                    var letter = new String(txtBoxR1.Text.Where(Char.IsLetter).ToArray());
                    if (letter == "k")
                    {
                        R1 = float.Parse(resultString) * 1000;
                    }
                    else if (letter == "M")
                    {
                        R1 = float.Parse(resultString) * 1000000;
                    }
                    else if (letter == "m")
                    {
                        R1 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter == "")
                    {
                        R1 = float.Parse(resultString);
                    }

                    /***************************************************/
                    resultString = Regex.Match(txtBoxR2.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxR2.Text, @"\d+").Value;
                    var letter2 = new String(txtBoxR2.Text.Where(Char.IsLetter).ToArray());
                    if (letter2 == "k")
                    {
                        R2 = float.Parse(resultString) * 1000;
                    }
                    else if (letter2 == "M")
                    {
                        R2 = float.Parse(resultString) * 1000000;
                    }
                    else if (letter2 == "m")
                    {
                        R2 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter2 == "") {
                        R2 = float.Parse(resultString);
                    }
                    /*********************************************************/
                    resultString = Regex.Match(txtBoxVout.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxVout.Text, @"\d+").Value;
                    var letter3 = new String(txtBoxVout.Text.Where(Char.IsLetter).ToArray());
                    if (letter3 == "u")
                    {
                        Vout = float.Parse(resultString) / 1000000;
                    }
                    else if (letter3 == "")
                    {
                        Vout = float.Parse(resultString);
                    }
                    else if (letter3 == "m")
                    {
                        Vout = float.Parse(resultString) / (float)1000.0;
                    }

                    Vcc = Vout*(R1+R2)/R2;
                    txtBoxVcc.Text = Vcc.ToString();
                }
                updateCurrent();
                updatePower();

            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnVout_Click(object sender, EventArgs e)
        {
            string resultString;
            try
            {
                if ((txtBoxVcc.Text != "") &&
                    (txtBoxR1.Text != "") &&
                     (txtBoxR2.Text != ""))
                {
                    Vcc = float.Parse(txtBoxVcc.Text);
                    resultString = Regex.Match(txtBoxR1.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxR1.Text, @"\d+").Value;
                    var letter = new String(txtBoxR1.Text.Where(Char.IsLetter).ToArray());
                    if (letter == "k")
                    {
                        R1 = float.Parse(resultString) * (float)1000.0;
                    }
                    else if (letter == "M")
                    {
                        R1 = float.Parse(resultString) * (float)1000000.0;
                    }
                    else if (letter == "m")
                    {
                        R1 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter == "")
                    {
                        R1 = float.Parse(resultString);
                    }

                    /***************************************************/
                    resultString = Regex.Match(txtBoxR2.Text, @"\d+[.]\d+").Value;
                    if(resultString == "") resultString = Regex.Match(txtBoxR2.Text, @"\d+").Value;
                    var letter2 = new String(txtBoxR2.Text.Where(Char.IsLetter).ToArray());
                    if (letter2 == "k")
                    {
                        R2 = float.Parse(resultString) * (float)1000.0;
                    }
                    else if (letter2 == "M")
                    {
                        R2 = float.Parse(resultString) * (float)1000000.0;
                    }
                    else if (letter2 == "m")
                    {
                        R2 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter2 == "")
                    {
                        R2 = float.Parse(resultString);
                    }

                    Vout = Vcc * R2 / (R1 + R2);


                    if (Vout < (float)0.001)
                    {
                        txtBoxVout.Text = (Vout * 1000000.0).ToString();
                        txtBoxVout.Text += "u";
                    }
                    else if (Vout < 1)
                    {
                        txtBoxVout.Text = (Vout * 1000.0).ToString();
                        txtBoxVout.Text += "m";
                    }
                    else {
                        txtBoxVout.Text = Vout.ToString();
                    }

                    

                }
                updateCurrent();
                updatePower();
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnR1_Click(object sender, EventArgs e)
        {
            string resultString;
            try
            {
                if ((txtBoxVcc.Text != "") &&                   
                    (txtBoxR2.Text != "") &&
                    (txtBoxVout.Text) != "")
                {
                    Vcc = float.Parse(txtBoxVcc.Text);
                    /***************************************************/
                    resultString = Regex.Match(txtBoxR2.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxR2.Text, @"\d+").Value;
                    var letter2 = new String(txtBoxR2.Text.Where(Char.IsLetter).ToArray());
                    if (letter2 == "k")
                    {
                        R2 = float.Parse(resultString) * (float)1000;
                    }
                    else if (letter2 == "M")
                    {
                        R2 = float.Parse(resultString) * (float)1000000;
                    }
                    else if (letter2 == "m")
                    {
                        R2 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter2 == "")
                    {
                        R2 = float.Parse(resultString);
                    }
                    /***********************************************/

                    resultString = Regex.Match(txtBoxVout.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxVout.Text, @"\d+").Value;
                    var letter3 = new String(txtBoxVout.Text.Where(Char.IsLetter).ToArray());
                    if (letter3 == "u")
                    {
                        Vout = float.Parse(resultString) / (float)1000000;
                    }
                    else if (letter3 == "")
                    {
                        Vout = float.Parse(resultString);
                    }
                    else if (letter3 == "m")
                    {
                        Vout = float.Parse(resultString) / (float)1000.0;
                    }
                    /*********************************************************/

                    R1 = (Vcc - Vout) * R2 / Vout;

                    if (R1 >= (float)1000000)
                    {
                        txtBoxR1.Text = (R1 / (float)1000000.0).ToString();
                        txtBoxR1.Text += "M";
                    }
                    else if (R1 >= (float)1000)
                    {
                        txtBoxR1.Text = (R1 / (float)1000.0).ToString();
                        txtBoxR1.Text += "k";
                    }

                    else if (R1 < (float)1)
                    {
                        txtBoxR1.Text = (R1 * (float)1000.0).ToString();
                        txtBoxR1.Text += "m";
                    }
                    else
                    {
                        txtBoxR1.Text = (R1).ToString();
                    }
                }
                updateCurrent();
                updatePower();

            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }


        }

        private void btnR2_Click(object sender, EventArgs e)
        {
            string resultString;
            try
            {
                if ((txtBoxVcc.Text != "") &&
                    (txtBoxR1.Text != "") &&                   
                    (txtBoxVout.Text) != "")
                {
                    Vcc = float.Parse(txtBoxVcc.Text);
                    resultString = Regex.Match(txtBoxR1.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxR1.Text, @"\d+").Value;
                    var letter = new String(txtBoxR1.Text.Where(Char.IsLetter).ToArray());
                    if (letter == "k")
                    {
                        R1 = float.Parse(resultString) * (float)1000;
                    }
                    else if (letter == "M")
                    {
                        R1 = float.Parse(resultString) * (float)1000000;
                    }
                    else if (letter == "m")
                    {
                        R1 = float.Parse(resultString) / (float)1000.0;
                    }
                    else if (letter == "")
                    {
                        R1 = float.Parse(resultString);
                    }
                    /***********************************************/

                    resultString = Regex.Match(txtBoxVout.Text, @"\d+[.]\d+").Value;
                    if (resultString == "") resultString = Regex.Match(txtBoxVout.Text, @"\d+").Value;
                    var letter3 = new String(txtBoxVout.Text.Where(Char.IsLetter).ToArray());
                    if (letter3 == "u")
                    {
                        Vout = float.Parse(resultString) / (float)1000000;
                    }
                    else if (letter3 == "")
                    {
                        Vout = float.Parse(resultString);
                    }
                    else if (letter3 == "m")
                    {
                        Vout = float.Parse(resultString) / (float)1000.0;
                    }

                    R2 = Vout * R1 / (Vcc - Vout);

                    /******************************************/
                    if (R2 >= (float)1000000)
                    {
                        txtBoxR2.Text = (R2 / (float)1000000.0).ToString();
                        txtBoxR2.Text += "M";
                    }
                    else if (R2 >= (float)1000)
                    {
                        txtBoxR2.Text = (R2 / (float)1000.0).ToString();
                        txtBoxR2.Text += "k";
                    }

                    else if (R2 < (float)1)
                    {
                        txtBoxR2.Text = (R2 * (float)1000.0).ToString();
                        txtBoxR2.Text += "m";
                    }
                    else
                    {
                        txtBoxR2.Text = (R2).ToString();
                    }

                }
                updateCurrent();
                updatePower();

            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        public void updateCurrent() {
            I = Vcc / (R1 + R2);

            if (I < (float)0.001)
            {
                lblCurrent.Text = (I * (float)1000000).ToString();
                lblCurrent.Text += "uA";
            }
            else if (I < (float)1) {
                lblCurrent.Text = (I*(float)1000).ToString();
                lblCurrent.Text += "mA";
            }
            else
            {
                lblCurrent.Text = (I * (float)1).ToString();
                lblCurrent.Text += "A";
            }



        }

        public void updatePower() {
            pR1 = I * I * R1;
            if (pR1 < 1)
            {
                lblPwrR1.Text = (pR1 * (float)1000).ToString();
                lblPwrR1.Text += "mW";
            }
            else if (pR1 >= (float)1000000)
            {
                lblPwrR1.Text = (pR1 / (float)1000000).ToString();
                lblPwrR1.Text += "MW";
            }
            else if (pR1 >= (float)1000)
            {
                lblPwrR1.Text = (pR1 / (float)1000).ToString();
                lblPwrR1.Text += "kW";
            }
            else {
                lblPwrR1.Text = (pR1 / (float)1).ToString();
                lblPwrR1.Text += "W";
            }

            /******************************************/
            pR2 = I * I * R2;
            if (pR2 < 1)
            {
                lblPwrR2.Text = (pR2 * (float)1000).ToString();
                lblPwrR2.Text += "mW";
            }
            else if (pR2 >= (float)1000000)
            {
                lblPwrR2.Text = (pR2 / (float)1000000).ToString();
                lblPwrR2.Text += "MW";
            }
            else if (pR2 >= (float)1000)
            {
                lblPwrR2.Text = (pR2 / (float)1000).ToString();
                lblPwrR2.Text += "kW";
            }
            else
            {
                lblPwrR2.Text = (pR2 / (float)1).ToString();
                lblPwrR2.Text += "W";
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Created by Vishwesh GM\nV1.0.0\nFor problem fixes, contact vishweshgm@outlook.com\nMob: +91 97385 69415", "Creator", MessageBoxButtons.OK, MessageBoxIcon.Information);

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }
}
