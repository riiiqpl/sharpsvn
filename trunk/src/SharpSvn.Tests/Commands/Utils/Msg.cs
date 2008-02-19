// $Id$
using System;

namespace SharpSvn.Tests.Commands.Utils
{
    /// <summary>
    /// Contains Win32 messages.
    /// </summary>
    public class Msg
    {
        /// <summary>
        /// Private ctor to avoid instantiation.
        /// </summary>
        private Msg()
        {			
        }

        /// <summary>
        /// Sets the normal or state image list for a tree-view control 
        /// and redraws the control using the new images. 
        /// </summary>
        /// <param name="wParam">Type of image list to set. 
        /// This parameter can be one of the following values: TVSIL_NORMAL or TVSIL_STATE</param>; 
        /// <param name="lParam">Handle to the image list. If himl is NULL, the message 
        /// removes the specified image list from the tree-view control.</param>         /// 
        /// <return>Returns the handle to the previous image list, if any, or NULL otherwise.</return>
        public const uint TVM_SETIMAGELIST = 0x1109;

        /// <summary>
        /// Retrieves the handle to the normal 
        /// or state image list associated with a tree-view control. 
        /// </summary>
        public const uint TVM_GETIMAGELIST = 4360;


        /// <summary>
        /// Retrieves some or all of a tree-view item's attributes. 
        /// </summary>
        public const uint TVM_GETITEM = 4364;

        /// <summary>
        /// The TVM_SETITEM message sets some or all of a tree-view item's attributes.
        /// </summary>
        public const uint TVM_SETITEM = 4365;

        /// <summary>
        /// Retrieves the tree-view item that bears the specified relationship to 
        /// a specified item. 
        /// </summary>
        public const uint TVM_GETNEXTITEM = 4362;

       
        /// <summary>
        /// Retrieves the edit control associated with a label edit (rename).
        /// </summary>
        public const uint TVM_GETEDITCONTROL = 0x110f;

        /// <summary>
        /// Sets an imagelist for a listview.
        /// </summary>
        public const uint LVM_SETIMAGELIST = 0x1003;

        public const uint TVN_ITEMEXPANDEDA = 0xfffffe6a;


        public const uint TVN_ITEMEXPANDEDW = 0xfffffe39;

        public const int WM_REFLECT = 0x00002000;
        public const int WM_NOTIFY = 0x0000004e;

        public const int TVM_ENSUREVISIBLE = 0x1114;
 



    }
}
