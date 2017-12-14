
package com.infotm.dv.adapter;

//Setting 区域头
public class SettingSection {
    private final String mId;
    private final String mLabel;

    public SettingSection(String id, String label) {
        this.mId = id;
        this.mLabel = label;
    }

    public boolean equals(Object tObject){
        if (this == tObject){
            return true;
        }
        SettingSection lSettingSection = (SettingSection) tObject;
        if (null != lSettingSection.mId && null != lSettingSection.mLabel){
            if (lSettingSection.mId.equals(mId) && lSettingSection.mLabel.equals(mLabel)){
                return true;
            }
        }
        return false;/*
    int i = 1;
    if (this == paramObject);
    while (true){
      SettingSection localSettingSection;
      label77: label96: 
      do
        while (true)
        {
          do
          {
            while (true)
            {
              while (true)
              {
                while (true)
                {
                  while (true)
                  {
                    return i;
                    if (paramObject != null)
                      break;
                    i = 0;
                  }
                  if (super.getClass() == paramObject.getClass())
                    break;
                  i = 0;
                }
                localSettingSection = (SettingSection)paramObject;
                if (this.mId != null)
                  break;
                if (localSettingSection.mId == null)
                  break label77;
                i = 0;
              }
              if (this.mId.equals(localSettingSection.mId))
                break;
              i = 0;
            }
            if (this.mLabel != null)
              break label96;
          }
          while (localSettingSection.mLabel == null;
          i = 0;
        }
      while (this.mLabel.equals(localSettingSection.mLabel);
      i = 0;
    }*/
  }

    public String getId() {
        return this.mId;
    }

    public String getLabel() {
        return this.mLabel;
    }

    // public int hashCode(){
    // int j;
    // int k;
    // int i = 0;
    // if (this.mId == null){
    // j = 0;
    // k = 31 * (j + 31);
    // if (this.mLabel != null)
    // break label41;
    // }
    // while (true){
    // while (true){
    // return (k + i);
    // j = this.mId.hashCode();
    // }
    // label41: i = this.mLabel.hashCode();
    // }
    // }

    public String toString() {
        return this.mLabel;
    }
}
