
var LibraryNativeImpl = {

	HasAnyEvents : function()
	{
		return Module.eventQueue.length;
	},

	PopCurrentEvent : function()
	{
		if(Module.eventQueue.length > 0) 
			Module.eventQueue.shift();
	},

	ExtractCurrentTouchX : function()
	{
		var ev = Module.eventQueue[0];
		return ev.gesture["center"].pageX;
	},
	
	ExtractCurrentTouchY : function()
	{
		var ev = Module.eventQueue[0];
		return ev.gesture["center"].pageY;
	},
	
	ExtractCurrentType : function()
	{
		var ev = Module.eventQueue[0];
		
		switch(ev.type)
		{
		case "touch": return 1;
		case "release": return 2;
		case "drag": return 3;
		default:
			return 0;
		}
	},
	
	GetCanvasWidth : function()
	{
		return $(Module.canvas).width();
	},
	
	GetCanvasHeight : function()
	{
		return $(Module.canvas).height();
	},
};

mergeInto(LibraryManager.library, LibraryNativeImpl);
