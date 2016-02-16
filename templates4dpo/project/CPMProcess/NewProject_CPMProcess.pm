# TemplateID=CPMProcess

package NewProject_CPMProcess;

use strict;

unshift(@INC, $ENV{DPO2_CORE_ROOT} . "/scripts");

require NewProjectTemplate;

use vars qw(@ISA);
@ISA = qw(NewProjectTemplate);


1;
