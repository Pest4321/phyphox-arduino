#include "phyphoxBleExperiment.h"

void PhyphoxBleExperiment::addView(View& v)
{
	for(int i=0; i<phyphoxBleNViews; i++)
	{
		if(VIEWS[i]==nullptr){
			VIEWS[i] = &v;
			break;
		}
	}
}

void PhyphoxBleExperiment::setTitle(const char *t){
	memset(&TITLE[0], 0, sizeof(TITLE));
	strcat(TITLE, t);
}

void PhyphoxBleExperiment::setCategory(const char *c){
	memset(&CATEGORY[0], 0, sizeof(CATEGORY));	
	strcat(CATEGORY, c);
}

void PhyphoxBleExperiment::setDescription(const char *d){
	memset(&DESCRIPTION[0], 0, sizeof(DESCRIPTION));
	strcat(DESCRIPTION, d);
}

void PhyphoxBleExperiment::setChannelForExport(int channel, char* label) {
    for (int i = 0; i < 6; i++) {
		if (exportConfiguration[i] == nullptr) {
            exportConfiguration[i] = new ExportChannel();
            exportConfiguration[i]->channel = channel;
            strcpy(exportConfiguration[i]->label, label);
            return;
		}
	}
}

void PhyphoxBleExperiment::autoDetectExportChannels() {
    for (int channel = 0; channel <= 5; channel++) {
        bool inUse = false;
        for (int i = 0; i < phyphoxBleNViews; i++){
	        inUse |= (VIEWS[i] != nullptr && VIEWS[i]->usesChannel(channel));
        }
        if (inUse) {
		    char label[9];
            if (channel == 0)
	            sprintf(label, "time (s)");
            else
	            sprintf(label, "CH%i", channel);
            setChannelForExport(channel, label);
        }
    }
}

void PhyphoxBleExperiment::getBytes(char *buffArray){
    //If no export configuration has been set by the user, we need to infer the used channels from the grpah configuration
    if (exportConfiguration[0] == nullptr) {
        autoDetectExportChannels();
    }

	//header
	strcat(buffArray, "<phyphox version=\"1.10\">\n");
	//build title
	strcat(buffArray,"<title>");
	strcat(buffArray, TITLE);
	strcat(buffArray,"</title>\n");
	
	//build category
	strcat(buffArray, "<category>");
	strcat(buffArray, CATEGORY);
	strcat(buffArray, "</category>\n");	

	//build description
	strcat(buffArray, "<description>");
	strcat(buffArray, DESCRIPTION);
	strcat(buffArray, "</description>\n");

	//build container
	strcat(buffArray, "<data-containers>\n");
	strcat(buffArray, "\t<container size=\"0\" static=\"false\">CH1</container>\n\t<container size=\"0\" static=\"false\">CH2</container>\n\t<container size=\"0\" static=\"false\">CH3</container>\n\t<container size=\"0\" static=\"false\">CH4</container>\n\t<container size=\"0\" static=\"false\">CH5</container>\n\t<container size=\"0\" static=\"false\">CH0</container>\n");
	strcat(buffArray, "</data-containers>\n");

	//build input
	strcat(buffArray, "<input>\n");
	strcat(buffArray, "\t<bluetooth mode=\"notification\" rate=\"1\" subscribeOnStart=\"false\">\n\t\t");

	for(int i=1; i<=5;i++){
		strcat(buffArray, "<output char=\"cddf1002-30f7-4671-8b43-5e40ba53514a\" conversion=\"float32LittleEndian\" ");
		char add[20];
		int k = (i-1)*4;
		sprintf(add, "offset=\"%i\" >CH%i", k,i);
		strcat(buffArray, add);
		strcat(buffArray,"</output>\n");
	}
	strcat(buffArray,"<output char=\"cddf1002-30f7-4671-8b43-5e40ba53514a\" extra=\"time\">CH0</output>");

	strcat(buffArray, "\t</bluetooth>\n");
	strcat(buffArray, "</input>\n");

	//build output
	strcat(buffArray, "<output></output>\n");

	//build analysis
	strcat(buffArray, "<analysis sleep=\"0\"  onUserInput=\"false\"></analysis>\n");
	
	//build views

	strcat(buffArray, "<views>\n");
	for(int i=0;i<phyphoxBleNViews; i++){
		if(VIEWS[i]!=nullptr){
			VIEWS[i]->getBytes(buffArray);
		}
	}
	strcat(buffArray,"</views>\n");

	//build export
	strcat(buffArray, "<export>\n");
	strcat(buffArray, "<set name=\"raw\">\n");
	for (int i = 0; i < 6; i++) {
		if (exportConfiguration[i] != nullptr) {
			char dataLine[60];
		    sprintf(dataLine, "<data name=\"%s\">CH%i</data>\n", exportConfiguration[i]->label, exportConfiguration[i]->channel);
        	strcat(buffArray, dataLine);
		}
	}
	strcat(buffArray, "</set>\n");
	strcat(buffArray, "</export>\n");
	
	//close
	strcat(buffArray, "</phyphox>");

}

