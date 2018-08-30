void test()
{ if(downButton.on() && selectButton.on())
  { while(1) 
    { delay(500); // weld every 10s
      weldCyclus(menuItems[2].upDownValueTable);
    }
  }
}


