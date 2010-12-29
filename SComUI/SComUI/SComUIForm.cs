using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SComUI
{
    public partial class SComUIForm : Form
    {
        public SComUIForm()
        {
            InitializeComponent();
        }

        private void SComUIFrom_Load(object sender, EventArgs e)
        {
            this.Text = "Loaded!";
        }
    }
}
