/**
******************************************************************************
 @file ostream.hpp

 @brief Usefull ostream operators with Qt classes

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/
#include <QSize>
#include <QString>

#include <ostream>

// This is outside the namespace to be accessible to all
//namespace QtExtra {

static std::ostream& operator<<(std::ostream& os, QSize const & size)
{
	os << "QSize(" << size.width() << ", " << size.height() << ")";
	return os;
}

static std::ostream& operator<<(std::ostream& os, const QString& str)
{
    os << std::string(str.toLatin1());
	return os;
}

//}
