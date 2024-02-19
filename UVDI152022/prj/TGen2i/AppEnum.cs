using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TGen2i.Enum.AppEnum
{
	/* Child Menu Dialog ID (enum_child_menu_dialog_id) */
	public enum ENG_CMDI : Byte /* unsigned char */
	{
		en_menu_work	= 0x01,		/* Work (Expose) Child Window */
		en_menu_recipe	= 0x02,		/* Recipe Child Window */
		en_menu_mc2		= 0x03,		/* Motion Controller Child Window */
		en_menu_plc		= 0x04,		/* PLC Child Window */
		en_menu_robot	= 0x05,		/* Diamond Robot Child Window */
		en_menu_aligner	= 0x06,		/* Diamond Prealigner Child Window */
		en_menu_logs	= 0x07,		/* Log Child Window */
		en_menu_config	= 0x08,		/* Configuration Child Window */
	}
}