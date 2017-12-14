#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/usb/otg.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <linux/usb/ehci_def.h>
#include <linux/device.h>
#include <mach/usb-phy.h>

struct imapx9_ehci_hcd {
	struct ehci_hcd *ehci;
	struct clk *bus_clk;
	struct clk *ref_clk;
};

static struct usb_hcd *imap_ehci_hcd;

uint32_t imap_ehci_port_state(void)
{
	uint32_t temp;
	struct ehci_hcd *ehci = hcd_to_ehci(imap_ehci_hcd);

	temp = ehci_readl(ehci, &ehci->regs->port_status[2]);
	return temp&0x1;
}

static int ehci_imapx800_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);

	ehci->caps = hcd->regs;

	/* switch to host mode */
	hcd->has_tt = 1;
	return ehci_setup(hcd);
}

static const struct hc_driver imapx9_ehci_driver = {
	.description = hcd_name,
	.product_desc = "Infotm Ehci Controller",
	.hcd_priv_size = sizeof(struct ehci_hcd),
	.irq = ehci_irq,
	.flags = HCD_USB2 | HCD_MEMORY,
	.reset = ehci_imapx800_init,
	.start = ehci_run,
	.stop = ehci_stop,
	.shutdown = ehci_shutdown,
	.urb_enqueue = ehci_urb_enqueue,
	.urb_dequeue = ehci_urb_dequeue,
	.endpoint_disable = ehci_endpoint_disable,
	.endpoint_reset = ehci_endpoint_reset,
	.get_frame_number = ehci_get_frame,
	.hub_status_data = ehci_hub_status_data,
	.hub_control = ehci_hub_control,
#if defined(CONFIG_PM)
	.bus_suspend = ehci_bus_suspend,
	.bus_resume = ehci_bus_resume,
#endif
	.relinquish_port = ehci_relinquish_port,
	.port_handed_over = ehci_port_handed_over,
	.clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};


static int imapx9_ehci_probe(struct platform_device *pdev)
{
	struct imapx9_ehci_hcd *imapx9;
	struct usb_hcd *hcd;
	struct resource *res;
	int rate;
	int err;
	int irq;

	if (usb_disabled())
		return -ENODEV;

	if (!pdev->dev.dma_mask)
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
	if (!pdev->dev.coherent_dma_mask)
		pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);

	imapx9 = devm_kzalloc(&pdev->dev, sizeof(struct imapx9_ehci_hcd),
				GFP_KERNEL);
	if (!imapx9)
		return -ENOMEM;

	hcd = usb_create_hcd(&imapx9_ehci_driver,
				&pdev->dev, dev_name(&pdev->dev));
	if (!hcd) {
		dev_err(&pdev->dev, "Unable to create HCD\n");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, imapx9);
#if 0
	imapx9->bus_clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(imapx9->bus_clk)) {
		dev_err(&pdev->dev, "Can't get ehci clock\n");
		err = PTR_ERR(imapx9->bus_clk);
		goto fail_bus_clk;
	}
	err = clk_prepare_enable(imapx9->bus_clk);
	if (err)
		goto fail_bus_clk;
#endif
	imapx9->bus_clk = clk_get_sys("imap-usb-ehci", "imap-usb");
	if (IS_ERR(imapx9->bus_clk)) {
		dev_err(&pdev->dev, "Can't get ehci clock\n");
		err = PTR_ERR(imapx9->bus_clk);
		goto fail_bus_clk;
	}
	err = clk_prepare_enable(imapx9->bus_clk);
	if (err)
		goto fail_bus_clk;

	imapx9->ref_clk = clk_get_sys("imap-usb-ref", "imap-usb");
	if (IS_ERR(imapx9->ref_clk)) {
		dev_err(&pdev->dev, "can't get phy ref clock\n");
		err = PTR_ERR(imapx9->ref_clk);
		goto fail_ref_clk;
	}
	clk_prepare_enable(imapx9->ref_clk);
	rate = clk_get_rate(imapx9->ref_clk);
	host_phy_config(rate, 1);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Failed to get I/O memory\n");
		err = -ENXIO;
		goto fail_io;
	}
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);
	hcd->regs = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!hcd->regs) {
		dev_err(&pdev->dev, "Failed to remap I/O memory\n");
		err = -ENOMEM;
		goto fail_io;
	}

	imapx9->ehci = hcd_to_ehci(hcd);
	irq = platform_get_irq(pdev, 0);
	if (!irq) {
		dev_err(&pdev->dev, "Failed to get IRQ\n");
		err = -ENODEV;
		goto fail_irq;
	}
	err = usb_add_hcd(hcd, irq, IRQF_DISABLED);
	if (err) {
		dev_err(&pdev->dev, "Failed to add USB HCD\n");
		goto fail_irq;
	}
	imap_ehci_hcd = hcd;
	return err;
fail_irq:
	devm_iounmap(&pdev->dev, hcd->regs);
fail_io:
	clk_disable_unprepare(imapx9->ref_clk);
fail_ref_clk:
	clk_disable_unprepare(imapx9->bus_clk);
fail_bus_clk:
	usb_put_hcd(hcd);
	return err;
}

static int imapx9_ehci_remove(struct platform_device *pdev)
{
	struct imapx9_ehci_hcd *imapx9 = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = ehci_to_hcd(imapx9->ehci);

	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);
	clk_disable_unprepare(imapx9->bus_clk);
	return 0;
}

static void imapx9_ehci_hcd_shutdown(struct platform_device *pdev)
{
	struct imapx9_ehci_hcd *imapx9 = platform_get_drvdata(pdev);
	struct usb_hcd *hcd = ehci_to_hcd(imapx9->ehci);

	if (hcd->driver->shutdown)
		hcd->driver->shutdown(hcd);
}

#ifdef CONFIG_PM
static int imapx9_ehci_hcd_suspend(struct device *dev)
{
	struct imapx9_ehci_hcd *imapx9 =
		platform_get_drvdata(to_platform_device(dev));
	struct ehci_hcd *ehci = imapx9->ehci;
	struct usb_hcd *hcd = ehci_to_hcd(ehci);

	return ehci_suspend(hcd, device_may_wakeup(dev));
}

static int imapx9_ehci_hcd_resume(struct device *dev)
{
	struct imapx9_ehci_hcd *imapx9 =
		platform_get_drvdata(to_platform_device(dev));
	struct ehci_hcd *ehci = imapx9->ehci;
	struct usb_hcd *hcd = ehci_to_hcd(ehci);

	ehci_resume(hcd, false);
	return 0;
}

static const struct dev_pm_ops imapx9_ehci_pm_ops = {
	.suspend	= imapx9_ehci_hcd_suspend,
	.resume		= imapx9_ehci_hcd_resume,
};
#endif

MODULE_ALIAS("platform:imap-ehci");

static struct platform_driver ehci_imapx9_driver = {
	.probe		= imapx9_ehci_probe,
	.remove		= imapx9_ehci_remove,
	.shutdown	= imapx9_ehci_hcd_shutdown,
	.driver		= {
		.name	= "imap-ehci",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm		= &imapx9_ehci_pm_ops,
#endif
	},
};
